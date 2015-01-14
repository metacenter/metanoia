// file: device-drm.c
// vim: tabstop=4 expandtab colorcolumn=81 list

// TODO: cleanup headers
#include "device-drm.h"
#include "utils-log.h"
#include "renderer-mmap.h"
#include "renderer-gl.h"

#include <errno.h>
#include <string.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <sys/mman.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <gbm.h>

#include "utils-dbus.h"
#include <malloc.h>
#include <libudev.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>
#include <pthread.h>

//------------------------------------------------------------------------------

#define INVALID_CRTC_ID 0

typedef struct {
    AuraOutput base;
    int fd;
    int front;
    uint32_t crtc_id;
    uint32_t connector_id;
    uint32_t fb[2];
    drmModeModeInfo mode;
} AuraOutputDRM;

static int fd = -1;

//------------------------------------------------------------------------------

static const char* scModuleName[] = {
        "i915", "radeon", "nouveau", "vmwgfx", "omapdrm", "exynos", "msm", NULL
    };

static const char* scConnectorTypeName[] = {
        "NONE", "VGA", "DVII", "DVID", "DVIA", "Composite", "SVIDEO", "LVDS",
        "Component", "9PinDIN", "DisplayPort", "HDMIA", "HDMIB", NULL
    };

static const char* scConnectorStateName[] = {
        "NONE", "connected", "disconnected", "Unknown", NULL
    };

pthread_mutex_t drm_mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

void aura_drm_log(int drm_fd, drmModeRes* resources)
{
    int i;

    // Log CRTC info
    for (i = 0; i < resources->count_crtcs; ++i) {
        drmModeCrtcPtr crtc = drmModeGetCrtc(drm_fd, resources->crtcs[i]);
        if (crtc) {
            LOG_INFO2("CRTC (id: %u, buffer: %u)",
                      crtc->crtc_id, crtc->buffer_id);
        }
        drmModeFreeCrtc(crtc);
    }

    // Log enocoder info
    for (i = 0; i < resources->count_encoders; ++i) {
        drmModeEncoderPtr encoder =
                              drmModeGetEncoder(drm_fd, resources->encoders[i]);
        if (encoder) {
            LOG_INFO2("Encoder (id: %u, CTRC: %u, mask: 0x%x)",
                       encoder->encoder_id, encoder->crtc_id,
                       encoder->possible_crtcs);
        }
        drmModeFreeEncoder(encoder);
    }

    // Log framebuffer info
    for (i = 0; i < resources->count_fbs; ++i) {
        drmModeFBPtr fb = drmModeGetFB(drm_fd, resources->fbs[i]);
        if (fb) {
            LOG_INFO2("Framebuffer (id: %u, w: %u, h: %u)",
                      fb->fb_id, fb->width, fb->height);
        }
        drmModeFreeFB(fb);
    }
}

//------------------------------------------------------------------------------
// DUMB BUFFERS

static int create_dumb_buffer(AuraOutputDRM* output_drm,
                              AuraRenderer* renderer,
                              int num)
{
    int result = 0;
    uint8_t* map;
    struct drm_mode_create_dumb carg;
    struct drm_mode_destroy_dumb darg;
    struct drm_mode_map_dumb marg;

    // Create dumb buffer
    memset(&carg, 0, sizeof(carg));
    carg.width  = output_drm->mode.hdisplay;
    carg.height = output_drm->mode.vdisplay;
    carg.bpp = 32;

    result = drmIoctl(output_drm->fd, DRM_IOCTL_MODE_CREATE_DUMB, &carg);
    if (result < 0) {
        LOG_ERROR("Cannot create dumb buffer (%m)");
        return -1;
    }

    // Create framebuffer object for the dumb-buffer
    result = drmModeAddFB(output_drm->fd, carg.width, carg.height,
                          24, carg.bpp, carg.pitch, carg.handle,
                          &output_drm->fb[num]);
    if (result) {
        LOG_ERROR("Cannot create framebuffer (%m)");
        goto clear_db;
    }

    LOG_DEBUG("FRAMEBUFFER %u", output_drm->fb[num]);

    // Prepare buffer for memory mapping
    memset(&marg, 0, sizeof(marg));
    marg.handle = carg.handle;
    result = drmIoctl(output_drm->fd, DRM_IOCTL_MODE_MAP_DUMB, &marg);
    if (result) {
        LOG_ERROR("Cannot map dumb buffer (%m)");
        goto clear_fb;
    }

    // Perform actual memory mapping
    map = mmap(0, carg.size, PROT_READ | PROT_WRITE,
               MAP_SHARED, output_drm->fd, marg.offset);

    if (map == MAP_FAILED) {
        LOG_ERROR("Cannot mmap dumb buffer (%m)");
        goto clear_fb;
    }

    aura_renderer_mmap_set_buffer(renderer, num, map, carg.pitch);
    return 0;

clear_fb:
    drmModeRmFB(output_drm->fd, output_drm->fb[num]);

clear_db:
    memset(&darg, 0, sizeof(darg));
    darg.handle = carg.handle;
    drmIoctl(output_drm->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &darg);

    return -1;
}

//------------------------------------------------------------------------------

static AuraRenderer* create_dumb_buffers(AuraOutputDRM* output_drm)
{
    AuraRenderer* renderer = aura_renderer_mmap_create((AuraOutput*) output_drm,
                                                       output_drm->base.width,
                                                       output_drm->base.height);
    create_dumb_buffer(output_drm, renderer, 0);
    create_dumb_buffer(output_drm, renderer, 1);

    return (AuraRenderer*) renderer;
}

//------------------------------------------------------------------------------
// EGL

AuraRenderer* initialize_egl_with_gbm(AuraOutputDRM* output_drm)
{
    /*int r; // TODO EGLint ?
    uint32_t fb_id;
    uint32_t width, height, stride, handle;
    EGLint major, minor, n;
    EGLDisplay egl_display;
    EGLConfig  egl_config;
    EGLContext egl_context;
    EGLSurface egl_surface;
    struct gbm_device*  gbm_device;
    struct gbm_surface* gbm_surface;
    struct gbm_bo*      gbm_bo;
    drmModeModeInfo* mode;

    static const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

    static const EGLint config_attribs[] = {
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,        1,
            EGL_GREEN_SIZE,      1,
            EGL_BLUE_SIZE,       1,
            EGL_ALPHA_SIZE,      0,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };

    LOG_INFO1("Creating GBM and initializing EGL...");

    // Find mode // TODO
    mode = &output_drm->connector->modes[0];

    // Create GBM device and surface
    gbm_device = gbm_create_device(output_drm->fd);

    gbm_surface = gbm_surface_create(gbm_device,
                                     mode->hdisplay, mode->vdisplay,
                                     GBM_FORMAT_XRGB8888,
                                     GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
    if (!gbm_surface) {
        LOG_ERROR("Failed to create GBM surface!");
        return NULL;
    }


    // Initialize EGL
    egl_display = eglGetDisplay((EGLNativeDisplayType) gbm_device);

    if (!eglInitialize(egl_display, &major, &minor)) {
        LOG_ERROR("Failed to initialize EGL display!");
        return NULL;
    }

    LOG_DATA2("EGL Version: '%s'", eglQueryString(egl_display, EGL_VERSION));
    LOG_DATA2("EGL Vendor:  '%s'", eglQueryString(egl_display, EGL_VENDOR));

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        LOG_ERROR("Failed to bind api EGL_OPENGL_ES_API!");
        return NULL;
    }

    r = eglChooseConfig(egl_display, config_attribs, &egl_config, 1, &n);
    if (!r || n != 1) {
        LOG_ERROR("Failed to choose EGL config (r: %d, n: %d)", r, n);
        return NULL;
    }

    egl_context = eglCreateContext(egl_display, egl_config,
                                   EGL_NO_CONTEXT, context_attribs);
    if (egl_context == NULL) {
        LOG_ERROR("Failed to create RGL context!");
        return NULL;
    }

    egl_surface = eglCreateWindowSurface(egl_display, egl_config,
                                         (EGLNativeWindowType) gbm_surface,
                                         NULL);
    if (egl_surface == EGL_NO_SURFACE) {
        LOG_ERROR("Failed to create EGL surface!");
        return NULL;
    }

    // Connect the context to the surface
    r = eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Failed to make EGL context current!");
        return NULL;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Something went wrong during EGL initialization!");
        return NULL;
    }

    // Create GBM BO
    eglSwapBuffers(egl_display, egl_surface);

    // TODO change to gbm_bo_create
    gbm_bo = gbm_surface_lock_front_buffer(gbm_surface);
    if (!gbm_bo) {
        LOG_ERROR("Could not lock GBM front buffer!");
        return NULL;
    }

    width = gbm_bo_get_width(gbm_bo);
    height = gbm_bo_get_height(gbm_bo);
    stride = gbm_bo_get_stride(gbm_bo);
    handle = gbm_bo_get_handle(gbm_bo).u32;

    r = drmModeAddFB(output_drm->fd, width, height,
                     24, 32, stride, handle, &fb_id);
    if (r) {
        LOG_ERROR("Failed to create DRM framebuffer: %m");
        return NULL;
    }

    // Set mode
    r = drmModeSetCrtc(output_drm->fd,
                       output_drm->crtc,
                       fb_id, 0, 0,
                       &output_drm->connector->connector_id,
                       1, mode);
    if (r) {
        LOG_ERROR("failed to set mode: %m");
        goto clear_fb;
    }

    gbm_surface_release_buffer(gbm_surface, gbm_bo); // TODO: remove?

    glClearColor(0.0, 0.25, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(egl_display, egl_surface);

    // Release context
    r = eglMakeCurrent(egl_display, EGL_NO_SURFACE,
                       EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_ERROR("Failed to release EGL context! (%d)", eglGetError());
    }

    LOG_INFO1("Creating GBM and initializing EGL: SUCCESS");

    return aura_renderer_gl_create(egl_display, egl_surface, egl_context);

clear_fb:
    drmModeRmFB(output_drm->fd, fb_id);*/
    return NULL;
}

//------------------------------------------------------------------------------
// OUTPUT

AuraRenderer* aura_drm_output_initialize(AuraOutput* output,
                                         int width, int height)
{
    AuraRenderer* renderer = NULL;

    AuraOutputDRM* output_drm = (AuraOutputDRM*) output;
    if (!output_drm) {
        return NULL;
    }

    //renderer = initialize_egl_with_gbm(output_drm);
    if (renderer == NULL) {
        renderer = create_dumb_buffers(output_drm);
    }
    if (renderer == NULL) {
        LOG_ERROR("DRM failed!");
        return NULL;
    }

    // Set mode
    int r = drmModeSetCrtc(output_drm->fd,
                           output_drm->crtc_id,
                           output_drm->fb[0], 0, 0,
                           &output_drm->connector_id, 1,
                           &output_drm->mode);
    if (r) {
        LOG_ERROR("Failed to set mode for connector "
                  "(id: %u, crtc_id: %u, fb0: %u, fb1: %u, error: '%m')",
                  output_drm->connector_id, output_drm->crtc_id,
                  output_drm->fb[0], output_drm->fb[1]);
    } else {
        LOG_INFO2("Set mode for connector "
                  "(id: %u, crtc_id: %u, fb0: %u, fb1: %u)",
                  output_drm->connector_id, output_drm->crtc_id,
                  output_drm->fb[0], output_drm->fb[1]);
    }

    return renderer;
}

//------------------------------------------------------------------------------

int aura_drm_output_swap_buffers(AuraOutput* output)
{
    pthread_mutex_lock(&drm_mutex);
    int result = 0;

    AuraOutputDRM* output_drm = (AuraOutputDRM*) output;
    if (!output_drm) {
        result = -1;
        goto unlock;
    }

    int new_front = output_drm->front ^ 1;

    int r = drmModeSetCrtc(output_drm->fd,
                           output_drm->crtc_id,
                           output_drm->fb[new_front], 0, 0,
                           &output_drm->connector_id, 1,
                           &output_drm->mode);
    if (r) {
        LOG_ERROR("Failed to set mode for connector "
                  "(id: %u, crtc_id: %u, error: '%m')",
                  output_drm->connector_id, output_drm->crtc_id);
        result = -1;
        goto unlock;
    }

    output_drm->front = new_front;

unlock:
    pthread_mutex_unlock(&drm_mutex);
    return result;
}

//------------------------------------------------------------------------------

void aura_drm_output_free(AuraOutput* output)
{
    if (!output) {
        return;
    }

    if (output->unique_name) {
        free(output->unique_name);
    }
    free(output);
}

//------------------------------------------------------------------------------

AuraOutputDRM* aura_drm_output_new(int width,
                                   int height,
                                   char* connector_name,
                                   int drm_fd,
                                   uint32_t crtc_id,
                                   uint32_t connector_id,
                                   drmModeModeInfo mode)
{
    AuraOutputDRM* output_drm = malloc(sizeof(AuraOutputDRM));
    memset(output_drm, 0, sizeof(AuraOutputDRM));

    aura_output_initialize(&output_drm->base,
                           width, height,
                           strdup(connector_name),
                           aura_drm_output_initialize,
                           aura_drm_output_swap_buffers,
                           aura_drm_output_free);

    output_drm->fd = drm_fd;
    output_drm->front = 0;
    output_drm->crtc_id = crtc_id;
    output_drm->connector_id = connector_id;
    output_drm->fb[0] = -1;
    output_drm->fb[1] = -1;
    output_drm->mode = mode;

    return output_drm;
}

//------------------------------------------------------------------------------
// DEVICE

int is_crtc_in_use(Chain* drm_outputs, uint32_t crtc_id)
{
    int is_in_use = 0;
    Link* link;
    for (link = drm_outputs->first; link; link = link->next) {
        AuraOutputDRM* output_drm = (AuraOutputDRM*) link->data;
        if (output_drm->crtc_id == crtc_id) {
            is_in_use = 1;
            break;
        }
    }
    return is_in_use;
}

//------------------------------------------------------------------------------

uint32_t find_crtc(int drm_fd,
                   drmModeRes* resources,
                   drmModeConnector* connector,
                   Chain* drm_outputs)
{
    // Try to reuse old encoder
    if (connector->encoder_id) {
        drmModeEncoderPtr encoder =
                               drmModeGetEncoder(drm_fd, connector->encoder_id);
        if (encoder
        &&  encoder->crtc_id != INVALID_CRTC_ID
        && !is_crtc_in_use(drm_outputs, encoder->crtc_id)) {
            return encoder->crtc_id;
        }
    }

    // If connector is not bound to encoder find new CRTC
    int i, j;
    for (i = 0; i < connector->count_encoders; ++i) {
        drmModeEncoderPtr encoder =
                              drmModeGetEncoder(drm_fd, connector->encoders[i]);
        for (j = 0; j < resources->count_crtcs; ++j) {
            if (!(encoder->possible_crtcs & (1 << j))) {
                continue;
            }

            drmModeCrtcPtr crtc = drmModeGetCrtc(drm_fd, resources->crtcs[j]);
            if (!crtc) {
                continue;
            }

            if (is_crtc_in_use(drm_outputs, crtc->crtc_id)) {
                continue;
            }

            uint32_t crtc_id = crtc->crtc_id;
            drmModeFreeCrtc(crtc);
            drmModeFreeEncoder(encoder);
            return crtc_id;
        }
        drmModeFreeEncoder(encoder);
    }

    return INVALID_CRTC_ID;
}

//------------------------------------------------------------------------------

AuraOutputDRM* update_device(int drm_fd,
                             drmModeRes* resources,
                             drmModeConnector* connector,
                             Chain* drm_outputs)
{
    LOG_INFO2("Updating connector (id: %u)", connector->connector_id);

    // Check if there is at least one valid mode
    if (connector->count_modes == 0) {
        LOG_ERROR("No valid mode!");
        return NULL;
    }

    uint32_t crtc_id = find_crtc(drm_fd, resources, connector, drm_outputs);
    if (crtc_id == INVALID_CRTC_ID) {
        LOG_ERROR("CRTC not found!");
        return NULL;
    }

    LOG_INFO2("Updating connector (CRTC: %u)", crtc_id);

    // Create output
    AuraOutputDRM* output =
     aura_drm_output_new(connector->modes[0].hdisplay,
                         connector->modes[0].vdisplay,
                         (char*) scConnectorTypeName[connector->connector_type],
                         drm_fd,
                         crtc_id,
                         connector->connector_id,
                         connector->modes[0]);
    return output;
}

//------------------------------------------------------------------------------

int aura_drm_update_devices(Chain* outputs)
{
    pthread_mutex_lock(&drm_mutex);

    unsigned int i;
    int j, num = -1;
    drmModeRes* resources = NULL;
    drmModeConnector* connector = NULL;
    Chain* drm_outputs = chain_new(NULL);

    LOG_INFO1("Updating DRM devices");

    // Find and open DRM device
    // TODO: try all devices, there may be more than one connected
    if (fd < 0) {
        for (i = 0; i < sizeof(scModuleName); ++i) {
            fd = drmOpen(scModuleName[i], NULL);
            if (fd > 0) {
                break;
            }
        }
        if (fd > 0) {
            LOG_INFO1("Found DRM device (%s)", scModuleName[i]);
        } else {
            LOG_ERROR("Could not open DRM device!");
            goto unlock;
        }
    }

    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        LOG_ERROR("drm device does not support dumb buffers");
        close(fd);
        goto unlock;
    }

    int ret = drmSetMaster(fd);
    if (ret == -1) {
        LOG_ERROR("Set Master: %m");
    }

    // Get resources
    resources = drmModeGetResources(fd);
    if (!resources) {
        LOG_ERROR("drmModeGetResources failed: %m");
        goto unlock;
    }

    aura_drm_log(fd, resources);

    num = 0;
    // Find a connected connectors
    for (j = 0; j < resources->count_connectors; ++j) {
        connector = drmModeGetConnector(fd, resources->connectors[j]);
        if (!connector) {
            continue;
        }

        LOG_INFO2("Connector: %s (%s)",
                  scConnectorTypeName[connector->connector_type],
                  scConnectorStateName[connector->connection]);

        // If connector is connected - update device
        if (connector->connection == DRM_MODE_CONNECTED) {
            AuraOutputDRM* output =
                           update_device(fd, resources, connector, drm_outputs);
            if (output) {
                chain_append(drm_outputs, output);
                num += 1;
            }
        }
        drmModeFreeConnector(connector);
    }

    Link* link;
    for (link = drm_outputs->first; link; link = link->next) {
        chain_append(outputs, link->data);
    }

    // Free memory
    drmModeFreeResources(resources);

unlock:
    pthread_mutex_unlock(&drm_mutex);
    return num;
}

//------------------------------------------------------------------------------

