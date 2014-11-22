// file: device-drm.c
// vim: tabstop=4 expandtab colorcolumn=81 list

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

//------------------------------------------------------------------------------

typedef struct {
    AuraOutput base;
    int fd;
    uint32_t crtc_id;
    drmModeConnector* connector;
} AuraOutputDRM;

//------------------------------------------------------------------------------

AuraRenderer* create_dumb_buffer(AuraOutputDRM* output_drm)
{
    int result = 0;
    uint8_t* map;
    uint32_t fb_id;
    struct drm_mode_create_dumb carg;
    struct drm_mode_destroy_dumb darg;
    struct drm_mode_map_dumb marg;

    // Create dumb buffer
    LOG_DEBUG("E: %d", errno);
    memset(&carg, 0, sizeof(carg));
    carg.width  = output_drm->connector->modes[0].hdisplay;
    carg.height = output_drm->connector->modes[0].vdisplay;
    carg.bpp = 32;


    result = drmIoctl(output_drm->fd, DRM_IOCTL_MODE_CREATE_DUMB, &carg);
    if (result < 0) {
        LOG_ERROR("Cannot create dumb buffer (%d): %s", errno, strerror(errno));
        return NULL;
    }
    LOG_DEBUG("S: %d*%d=%d", carg.pitch, carg.height, carg.pitch*carg.height);
    LOG_DEBUG("H: %d, %d", carg.size, carg.handle);

    // Create framebuffer object for the dumb-buffer
    result = drmModeAddFB(output_drm->fd, carg.width, carg.height, 24,
                          carg.bpp, carg.pitch, carg.handle, &fb_id);
    if (result) {
        LOG_ERROR("Cannot create framebuffer (%d): %s", errno, strerror(errno));
        goto clear_db;
    }

    LOG_DEBUG("H: %d, %d", carg.size, carg.handle);
    // Prepare buffer for memory mapping
    memset(&marg, 0, sizeof(marg));
    marg.handle = carg.handle;
    result = drmIoctl(output_drm->fd, DRM_IOCTL_MODE_MAP_DUMB, &marg);
    if (result) {
        LOG_ERROR("Cannot map dumb buffer (%d): %s", errno, strerror(errno));
        goto clear_fb;
    }

    LOG_DEBUG("H: %d %d", carg.size, marg.offset);
    LOG_DEBUG("E: %d", errno);
    // Perform actual memory mapping
    map = mmap(0, carg.size, PROT_READ | PROT_WRITE,
               MAP_SHARED, output_drm->fd, marg.offset);

    LOG_DEBUG("E: %d", errno);
    if (map == MAP_FAILED) {
        LOG_ERROR("Cannot mmap dumb buffer (%d): %s", errno, strerror(errno));
        result = -errno;
        goto clear_fb;
    }

    // Set mode
    result = drmModeSetCrtc(output_drm->fd,
                            output_drm->crtc_id,
                            fb_id, 0, 0,
                            &output_drm->connector->connector_id,
                            1, &output_drm->connector->modes[0]);
    if (result) {
        LOG_ERROR("failed to set mode: %s\n", strerror(errno));
        goto clear_fb;
    }

    // TODO: remove
    int width = carg.width;
    int height = carg.height;
    int pitch = carg.pitch;
    LOG_DEBUG(" %d %d %d", width, height, pitch);
    int x, y;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            if (x < width/3){
                map[y*pitch + 4*x + 0] = 0x00;
                map[y*pitch + 4*x + 1] = 0xFF;
                map[y*pitch + 4*x + 2] = 0xFF;
                map[y*pitch + 4*x + 3] = 0xFF;
            }
            else if (x < width*2/3) {
                map[y*pitch + 4*x + 0] = 0xFF;
                map[y*pitch + 4*x + 1] = 0xFF;
                map[y*pitch + 4*x + 2] = 0x00;
                map[y*pitch + 4*x + 3] = 0xFF;
            }
            else {
                map[y*pitch + 4*x + 0] = 0xFF;
                map[y*pitch + 4*x + 1] = 0x00;
                map[y*pitch + 4*x + 2] = 0xFF;
                map[y*pitch + 4*x + 3] = 0xFF;
            }
        }
    }
    return aura_renderer_mmap_create(map, carg.width, carg.height, carg.pitch);

clear_fb:
    drmModeRmFB(output_drm->fd, fb_id);

clear_db:
    memset(&darg, 0, sizeof(darg));
    darg.handle = carg.handle;
    drmIoctl(output_drm->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &darg);

    return NULL;
}

//------------------------------------------------------------------------------

AuraRenderer* initialize_egl_with_gbm(AuraOutputDRM* output_drm)
{
    int r; // TODO EGLint ?
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
        LOG_ERROR("Failed to create DRM framebuffer: %s\n", strerror(errno));
        return NULL;
    }

    // Set mode
    r = drmModeSetCrtc(output_drm->fd,
                       output_drm->crtc_id,
                       fb_id, 0, 0,
                       &output_drm->connector->connector_id,
                       1, mode);
    if (r) {
        LOG_ERROR("failed to set mode: %s\n", strerror(errno));
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
    drmModeRmFB(output_drm->fd, fb_id);
    return NULL;
}

//------------------------------------------------------------------------------

AuraRenderer* aura_drm_output_initialize(struct AuraOutput* output,
                                         int widht, int height)
{
    AuraOutputDRM* output_drm;
    AuraRenderer* renderer;

    if (!output) {
        return NULL;
    }

    errno = 0;
    LOG_DEBUG("E: %d", errno);
    output_drm = (AuraOutputDRM*) output;

    //renderer = initialize_egl_with_gbm(output_drm);
    //if (renderer == NULL) {
        renderer = create_dumb_buffer(output_drm);
        if (renderer == NULL) {
            LOG_ERROR("DRM failed!");
            return NULL;
        }
    //}

    // TODO: set mode (only) here

    return renderer;
}

//------------------------------------------------------------------------------

int update_device(int drm_fd,
                  drmModeRes* resources,
                  drmModeConnector* connector,
                  AuraOutput** output)
{
    int i;
    drmModeEncoder* encoder = NULL;

    LOG_INFO2("Updating connector %u", connector->connector_id);

    // Check if there is at least one valid mode
    if (connector->count_modes == 0) {
        LOG_ERROR("No valid mode!");
        return -EFAULT;
    }

    // Find encoder
    for (i = 0; i < resources->count_encoders; ++i) {
        encoder = drmModeGetEncoder(drm_fd, resources->encoders[i]);
        if (encoder && encoder->encoder_id == connector->encoder_id) {
            break;
        }
        drmModeFreeEncoder(encoder);
        encoder = NULL;
    }

    if (!encoder) {
        LOG_ERROR("No encoder!");
        return -EFAULT;
    }

    AuraOutputDRM* output_drm = malloc(sizeof(AuraOutputDRM));
    memset(output_drm, 0, sizeof(AuraOutputDRM));

    output_drm->base.width = connector->modes[0].vdisplay;
    output_drm->base.height = connector->modes[0].hdisplay;
    output_drm->base.initialize = aura_drm_output_initialize;

    output_drm->fd = drm_fd;
    output_drm->connector = connector;
    output_drm->crtc_id = encoder->crtc_id;

    *output = (AuraOutput*) output_drm;

    drmModeFreeEncoder(encoder);
    return 1;
}

//------------------------------------------------------------------------------

int aura_drm_update_devices(AuraOutput** outputs, int* num)
{
    drmModeRes* resources = NULL;
    drmModeConnector* connector = NULL;
    int fd, i;

    LOG_INFO1("Updating DRM devices");

    // Find and open DRM device
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
        return -1;
    }

/*char node[] = "/dev/dri/card0";

struct stat st;
int r = stat(node, &st);
if (r >= 0) {
    fd = aura_dbus_session_take_device(major(st.st_rdev), minor(st.st_rdev));
    LOG_DEBUG("FD: %d", fd);
} else {
    LOG_ERROR("NO STAT");
}*/

    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        LOG_ERROR("drm device does not support dumb buffers");
        close(fd);
        return -1;
    }

int ret = drmSetMaster(fd);
LOG_DEBUG("MASTER: %d", ret);
if (ret == -1) {
    LOG_ERROR("Set Master: %s", strerror(errno));
}

    // Get resources
    resources = drmModeGetResources(fd);
    if (!resources) {
        LOG_ERROR("drmModeGetResources failed: %s", strerror(errno));
        return -1;
    }

    // TODO: put malloc elsewhere
    *outputs = malloc(sizeof(AuraOutput));
    (*outputs)->width = 1366;
    (*outputs)->height = 768;
    *num = 1;

    // Find a connected connector
    for (i = 0; i < resources->count_connectors; ++i) {
        connector = drmModeGetConnector(fd, resources->connectors[i]);
        if (connector)
        {
            LOG_INFO2("Connector: %s (%s)",
                       scConnectorTypeName[connector->connector_type],
                       scConnectorStateName[connector->connection]);
            // If connector is connected - update device
            if (connector->connection == DRM_MODE_CONNECTED) {
                update_device(fd, resources, connector, outputs);
            }
            else
            {
                drmModeFreeConnector(connector);
            }
        }
        connector = NULL;
    }

    // Free memory
    drmModeFreeResources(resources);

    return 1;
}

