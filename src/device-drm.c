// file: device-drm.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "device-drm.h"
#include "utils-log.h"
#include "renderer-mmap.h"
#include "renderer-gl.h"
#include "config.h"

#include <sys/mman.h>
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//------------------------------------------------------------------------------

#define INVALID_CRTC_ID 0
#define INVALID_FB_ID 0

/// Data type used to bind DRM frame buffer information to GBM buffer object.
typedef struct {
    uint32_t fb;
} NoiaGBMBundle;

/// DRM subtype of Output
/// @see NoiaOutput
typedef struct {
    NoiaOutput base;
    int fd;
    int front;
    uint32_t crtc_id;
    uint32_t connector_id;
    uint32_t fb[2];
    struct gbm_surface* gbm_surface;
    struct gbm_bo* gbm_bo;
    drmModeModeInfo mode;
} NoiaOutputDRM;

//------------------------------------------------------------------------------

/// Names of kernel modules to look up.
static const char* scModuleName[] = {
        "i915", "radeon", "nouveau", "vmwgfx", "omapdrm", "exynos", "msm", NULL
    };

/// Connector state dictionary
static const char* scConnectorStateName[] = {
        "NONE", "connected", "disconnected", "Unknown", NULL
    };

pthread_mutex_t drm_mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

/// Translate connector type to human-readable name.
char* noia_drm_get_connector_name(uint32_t connector_type)
{
    static const char* scConnectorTypeName[] = {
            "Unknown", "VGA", "DVII", "DVID", "DVIA", "Composite",
            "SVIDEO", "LVDS", "Component", "9PinDIN", "DisplayPort",
            "HDMIA", "HDMIB", "TV", "eDP"
        };

    if ((connector_type < 1)
    or  (connector_type >= sizeof(scConnectorTypeName))) {
        LOG_ERROR("Connector type '%u' out of range!", connector_type);
        return strdup("NONE");
    }
    return strdup(scConnectorTypeName[connector_type]);
}

//------------------------------------------------------------------------------

/// Log detailed information about devices.
void noia_drm_log(int drm_fd, drmModeRes* resources)
{
    // Log CRTC info
    for (int i = 0; i < resources->count_crtcs; ++i) {
        drmModeCrtcPtr crtc = drmModeGetCrtc(drm_fd, resources->crtcs[i]);
        if (crtc) {
            LOG_INFO2("CRTC (id: %u, buffer: %u)",
                      crtc->crtc_id, crtc->buffer_id);
        }
        drmModeFreeCrtc(crtc);
    }

    // Log encoder info
    for (int i = 0; i < resources->count_encoders; ++i) {
        drmModeEncoderPtr encoder =
                              drmModeGetEncoder(drm_fd, resources->encoders[i]);
        if (encoder) {
            LOG_INFO2("Encoder (id: %u, CRTC: %u, mask: 0x%x)",
                       encoder->encoder_id, encoder->crtc_id,
                       encoder->possible_crtcs);
        }
        drmModeFreeEncoder(encoder);
    }

    // Log frame buffer info
    for (int i = 0; i < resources->count_fbs; ++i) {
        drmModeFBPtr fb = drmModeGetFB(drm_fd, resources->fbs[i]);
        if (fb) {
            LOG_INFO2("Framebuffer (id: %u, w: %u, h: %u)",
                      fb->fb_id, fb->width, fb->height);
        }
        drmModeFreeFB(fb);
    }
}

//------------------------------------------------------------------------------
// GBM

/// Create DRM frame buffer for use with GBM.
/// @see NoiaGBMBundle
NoiaGBMBundle* noia_drm_gbm_create(int drm_fd, struct gbm_bo* bo)
{
    NoiaGBMBundle* bundle = malloc(sizeof(NoiaGBMBundle));
    if (not bundle) {
        return NULL;
    }

    uint32_t width  = gbm_bo_get_width(bo);
    uint32_t height = gbm_bo_get_height(bo);
    uint32_t stride = gbm_bo_get_stride(bo);
    uint32_t handle = gbm_bo_get_handle(bo).u32;

    int r = drmModeAddFB(drm_fd, width, height, 24, 32,
                         stride, handle, &bundle->fb);
    if (r) {
        LOG_ERROR("Failed to create DRM framebuffer: %m");
        return NULL;
    }

    return bundle;
}

//------------------------------------------------------------------------------

/// Destructor
void noia_drm_gbm_destroy(NoiaGBMBundle* bundle)
{
    if (bundle) {
        free(bundle);
    }
}

//------------------------------------------------------------------------------

/// GBM user data destruction notification handler.
void noia_drm_gbm_destroy_event(struct gbm_bo *bo, void *data NOIA_UNUSED)
{
    LOG_INFO2("Destroing GBM buffer object!");
    NoiaGBMBundle* bundle = gbm_bo_get_user_data(bo);
    if (bundle) {
        noia_drm_gbm_destroy(bundle);
    }
}

//------------------------------------------------------------------------------

/// Lock GBM surface and create new frame buffer if needed.
uint32_t noia_drm_gbm_lock_surface(NoiaOutputDRM* output_drm)
{
    if (not output_drm->gbm_surface) {
        LOG_ERROR("Invalid GBM surface!");
        return INVALID_FB_ID;
    }

    struct gbm_bo* bo = gbm_surface_lock_front_buffer(output_drm->gbm_surface);
    if (not bo) {
        LOG_ERROR("Could not lock GBM front buffer!");
    } else {
        output_drm->gbm_bo = bo;
    }

    NoiaGBMBundle* bundle = gbm_bo_get_user_data(output_drm->gbm_bo);
    if (not bundle) {
        bundle = noia_drm_gbm_create(output_drm->fd, output_drm->gbm_bo);
        gbm_bo_set_user_data(output_drm->gbm_bo, bundle,
                             noia_drm_gbm_destroy_event);
    }

    return bundle->fb;
}

//------------------------------------------------------------------------------

/// Prepare GL render for use with GBM buffers.
NoiaRenderer* noia_drm_initialize_egl(NoiaOutputDRM* output_drm)
{
    NoiaEGLBundle egl;
    struct gbm_device* gbm_device;

    LOG_INFO1("Creating GBM and initializing EGL...");

    /// @todo Find mode
    NoiaSize size = {output_drm->mode.hdisplay, output_drm->mode.vdisplay};

    // Create GBM device and surface
    gbm_device = gbm_create_device(output_drm->fd);
    if (not gbm_device) {
        LOG_ERROR("Failed to create GBM device!");
        return NULL;
    }

    output_drm->gbm_surface =
                  gbm_surface_create(gbm_device,
                                     size.width, size.height,
                                     GBM_FORMAT_XRGB8888,
                                     GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    if (not output_drm->gbm_surface) {
        LOG_ERROR("Failed to create GBM surface!");
        return NULL;
    }

    // Initiate EGL
    NoiaResult result = noia_gl_create_onscreen_egl_bundle
                                 ((EGLNativeDisplayType) gbm_device,
                                  (EGLNativeWindowType) output_drm->gbm_surface,
                                   &egl);
    if (result != NOIA_RESULT_SUCCESS) {
        return NULL;
    }

    LOG_INFO1("Creating GBM and initializing EGL: SUCCESS");

    return noia_renderer_gl_create(&egl, size);
}

//------------------------------------------------------------------------------
// DUMB BUFFERS

/// Create single DRM dumb buffer for use with renderer.
NoiaResult noia_drm_create_dumb_buffer(NoiaOutputDRM* output_drm,
                                       NoiaRenderer* renderer,
                                       int num)
{
    int res;
    uint8_t* map = NULL;
    struct drm_mode_create_dumb carg;
    struct drm_mode_destroy_dumb darg;
    struct drm_mode_map_dumb marg;
    NoiaResult result = NOIA_RESULT_ERROR;
    output_drm->fb[num] = 0;

    NOIA_BLOCK {
        // Create dumb buffer
        memset(&carg, 0, sizeof(carg));
        carg.width  = output_drm->mode.hdisplay;
        carg.height = output_drm->mode.vdisplay;
        carg.bpp = 32;

        res = drmIoctl(output_drm->fd, DRM_IOCTL_MODE_CREATE_DUMB, &carg);
        if (res < 0) {
            LOG_ERROR("Cannot create dumb buffer (%m)");
            break;
        }

        // Create framebuffer object for the dumb-buffer
        res = drmModeAddFB(output_drm->fd, carg.width, carg.height,
                           24, carg.bpp, carg.pitch, carg.handle,
                           &output_drm->fb[num]);
        if (res) {
            LOG_ERROR("Cannot create framebuffer (%m)");
            break;
        }

        // Prepare buffer for memory mapping
        memset(&marg, 0, sizeof(marg));
        marg.handle = carg.handle;
        res = drmIoctl(output_drm->fd, DRM_IOCTL_MODE_MAP_DUMB, &marg);
        if (res) {
            LOG_ERROR("Cannot map dumb buffer (%m)");
            break;
        }

        // Perform actual memory mapping
        map = mmap(0, carg.size, PROT_READ | PROT_WRITE,
                   MAP_SHARED, output_drm->fd, marg.offset);

        if (map == MAP_FAILED) {
            LOG_ERROR("Cannot mmap dumb buffer (%m)");
            break;
        }

        noia_renderer_mmap_set_buffer(renderer, num, map, carg.pitch);
        result = NOIA_RESULT_SUCCESS;
        LOG_INFO2("Created dumb buffer (fb: %u)", output_drm->fb[num]);
    }

    // Clean up
    if (result != NOIA_RESULT_SUCCESS) {
        if (output_drm->fb[num]) {
            drmModeRmFB(output_drm->fd, output_drm->fb[num]);
            output_drm->fb[num] = 0;
        }

        if (carg.handle) {
            memset(&darg, 0, sizeof(darg));
            darg.handle = carg.handle;
            drmIoctl(output_drm->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &darg);
        }
    }

    return result;
}

//------------------------------------------------------------------------------

/// Prepare MMap render for use with DRM dumb buffers.
NoiaRenderer* noia_drm_create_dumb_buffers(NoiaOutputDRM* output_drm)
{
    NoiaRenderer* renderer = noia_renderer_mmap_create(&output_drm->base);
    noia_drm_create_dumb_buffer(output_drm, renderer, 0);
    noia_drm_create_dumb_buffer(output_drm, renderer, 1);

    return (NoiaRenderer*) renderer;
}

//------------------------------------------------------------------------------
// OUTPUT

/// Prepare output for rendering.
/// Depending on hardware and settings create renderer for drawing with GL
/// or on DRM dumb buffers.
NoiaRenderer* noia_drm_output_initialize(NoiaOutput* output,
                                         NoiaSize size NOIA_UNUSED)
{
    NoiaRenderer* renderer = NULL;
    NoiaOutputDRM* output_drm = (NoiaOutputDRM*) output;
    NOIA_ENSURE(output_drm, return renderer);

    if (noia_settings()->use_gl) {
        renderer = noia_drm_initialize_egl(output_drm);
    }

    if (renderer == NULL) {
        renderer = noia_drm_create_dumb_buffers(output_drm);
    }

    if (renderer == NULL) {
        LOG_ERROR("DRM failed!");
    }
    return renderer;
}

//------------------------------------------------------------------------------

/// Swap buffers.
/// This function is used only for dumb buffers.
/// @see noia_drm_output_end_drawing, noia_drm_gbm_lock_surface
uint32_t noia_drm_output_swap_buffers(NoiaOutputDRM* output_drm)
{
    output_drm->front = output_drm->front ^ 1;
    return output_drm->fb[output_drm->front];
}

//------------------------------------------------------------------------------

/// Release GBM buffer locked with `noia_drm_gbm_lock_surface`.
/// @see noia_drm_gbm_lock_surface
NoiaResult noia_drm_output_begin_drawing(NoiaOutput* output)
{
    NoiaOutputDRM* output_drm = (NoiaOutputDRM*) output;
    NOIA_ENSURE(output_drm, return NOIA_RESULT_INCORRECT_ARGUMENT);

    if (output_drm->gbm_surface and output_drm->gbm_bo) {
        gbm_surface_release_buffer(output_drm->gbm_surface, output_drm->gbm_bo);
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// End drawing - swap buffers and set mode.
NoiaResult noia_drm_output_end_drawing(NoiaOutput* output)
{
    NoiaOutputDRM* output_drm = (NoiaOutputDRM*) output;
    NOIA_ENSURE(output_drm, return NOIA_RESULT_INCORRECT_ARGUMENT);

    pthread_mutex_lock(&drm_mutex);
    NoiaResult result = NOIA_RESULT_ERROR;

    int32_t fb = output_drm->gbm_surface
               ? noia_drm_gbm_lock_surface(output_drm)
               : noia_drm_output_swap_buffers(output_drm);

    int r = drmModeSetCrtc(output_drm->fd,
                           output_drm->crtc_id,
                           fb, 0, 0,
                           &output_drm->connector_id, 1,
                           &output_drm->mode);
    if (r) {
        LOG_ERROR("Failed to set mode for connector "
                  "(id: %u, crtc_id: %u, error: '%m')",
                  output_drm->connector_id, output_drm->crtc_id);
    } else {
        result = NOIA_RESULT_SUCCESS;
    }

    pthread_mutex_unlock(&drm_mutex);
    return result;
}

//------------------------------------------------------------------------------

/// DRM Output destructor.
void noia_drm_output_free(NoiaOutput* output)
{
    NOIA_ENSURE(output, return);
    if (output->unique_name) {
        free(output->unique_name);
    }
    free(output);
}

//------------------------------------------------------------------------------

/// DRM output constructor.
NoiaOutputDRM* noia_drm_output_new(NoiaSize size,
                                   char* connector_name,
                                   int drm_fd,
                                   uint32_t crtc_id,
                                   uint32_t connector_id,
                                   drmModeModeInfo mode)
{
    NoiaOutputDRM* output_drm = calloc(1, sizeof(NoiaOutputDRM));
    NOIA_ENSURE(output_drm, abort());

    noia_output_initialize(&output_drm->base,
                           size,
                           strdup(connector_name ? connector_name : ""),
                           noia_drm_output_initialize,
                           noia_drm_output_begin_drawing,
                           noia_drm_output_end_drawing,
                           noia_drm_output_free);

    output_drm->fd = drm_fd;
    output_drm->front = 0;
    output_drm->crtc_id = crtc_id;
    output_drm->connector_id = connector_id;
    output_drm->fb[0] = -1;
    output_drm->fb[1] = -1;
    output_drm->mode = mode;
    output_drm->gbm_surface = NULL;
    output_drm->gbm_bo = NULL;

    return output_drm;
}

//------------------------------------------------------------------------------
// DEVICE

/// This function check if CRTC with id `crtc_id` is already in use.
bool noia_drm_is_crtc_in_use(NoiaList* drm_outputs, uint32_t crtc_id)
{
    int is_in_use = false;
    FOR_EACH (drm_outputs, link) {
        NoiaOutputDRM* output_drm = (NoiaOutputDRM*) link->data;
        if (output_drm->crtc_id == crtc_id) {
            is_in_use = true;
            break;
        }
    }
    return is_in_use;
}

//------------------------------------------------------------------------------

/// This function matches CRTC to given connector.
uint32_t noia_drm_find_crtc(int drm_fd,
                            drmModeRes* resources,
                            drmModeConnector* connector,
                            NoiaList* drm_outputs)
{
    // Try to reuse old encoder
    if (connector->encoder_id) {
        drmModeEncoderPtr encoder =
                               drmModeGetEncoder(drm_fd, connector->encoder_id);
        if (encoder
        and (encoder->crtc_id != INVALID_CRTC_ID)
        and (not noia_drm_is_crtc_in_use(drm_outputs, encoder->crtc_id))) {
            return encoder->crtc_id;
        }
    }

    // If connector is not bound to encoder find new CRTC
    for (int i = 0; i < connector->count_encoders; ++i) {
        drmModeEncoderPtr encoder =
                              drmModeGetEncoder(drm_fd, connector->encoders[i]);
        for (int j = 0; j < resources->count_crtcs; ++j) {
            if (not (encoder->possible_crtcs & (1 << j))) {
                continue;
            }

            drmModeCrtcPtr crtc = drmModeGetCrtc(drm_fd, resources->crtcs[j]);
            if (not crtc) {
                continue;
            }

            if (noia_drm_is_crtc_in_use(drm_outputs, crtc->crtc_id)) {
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

/// Construct Output instance for work with given connector.
NoiaOutputDRM* noia_drm_update_connector(int drm_fd,
                                         drmModeRes* resources,
                                         drmModeConnector* connector,
                                         NoiaList* drm_outputs)
{
    if (not connector) {
        return NULL;
    }

    LOG_INFO2("Updating connector (id: %u, type: %s, state: %s)",
              connector->connector_id,
              noia_drm_get_connector_name(connector->connector_type),
              scConnectorStateName[connector->connection]);

    // Check if connector is connected
    if (connector->connection != DRM_MODE_CONNECTED) {
        return NULL;
    }

    // Check if there is at least one valid mode
    if (connector->count_modes == 0) {
        LOG_ERROR("No valid mode!");
        return NULL;
    }

    uint32_t crtc_id = noia_drm_find_crtc(drm_fd, resources,
                                          connector, drm_outputs);
    if (crtc_id == INVALID_CRTC_ID) {
        LOG_ERROR("CRTC not found!");
        return NULL;
    }

    LOG_INFO2("Updating connector (CRTC: %u)", crtc_id);

    // Create output
    NoiaSize size = {connector->modes[0].hdisplay,
                     connector->modes[0].vdisplay};
    NoiaOutputDRM* output =
     noia_drm_output_new(size,
                         noia_drm_get_connector_name(connector->connector_type),
                         drm_fd,
                         crtc_id,
                         connector->connector_id,
                         connector->modes[0]);
    return output;
}

//------------------------------------------------------------------------------

/// Scan devices and return list of available Outputs.
/// @param[out] outputs - list out the outputs
/// @return number of outputs found.
/// @see noia_output_collector_fetch_actual_outputs,
///      noia_devfb_setup_framebuffer
int noia_drm_update_devices(NoiaList* outputs)
{
    pthread_mutex_lock(&drm_mutex);

    int ret;
    int num = 0;
    int fd = -1;
    drmModeRes* resources = NULL;
    drmModeConnector* connector = NULL;
    NoiaList* drm_outputs = noia_list_new(NULL);

    LOG_INFO1("Updating DRM devices");

    NOIA_BLOCK {
        // Find and open DRM device
        /// @todo: try all devices, there may be more than one connected
        unsigned int i;
        for (i = 0; scModuleName[i]; ++i) {
            fd = drmOpen(scModuleName[i], NULL);
            if (fd > 0) {
                break;
            }
        }
        if (fd > 0) {
            LOG_INFO1("Found DRM device (%s)", scModuleName[i]);
        } else {
            LOG_ERROR("Could not open DRM device!");
            break;
        }

        // Check if device supports dumb buffers
        uint64_t has_dumb = false;
        ret = drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb);
        if ((ret < 0) or (not has_dumb)) {
            LOG_ERROR("DRM device does not support dumb buffers!");
            close(fd);
            break;
        }

        // Try to set master
        ret = drmSetMaster(fd);
        if (ret == -1) {
            LOG_WARN1("Set Master: %m");
        }

        // Get resources
        resources = drmModeGetResources(fd);
        if (not resources) {
            LOG_ERROR("drmModeGetResources failed: %m");
            break;
        }

        noia_drm_log(fd, resources);

        // Find connected connectors
        for (int i = 0; i < resources->count_connectors; ++i) {
            connector = drmModeGetConnector(fd, resources->connectors[i]);

            NoiaOutputDRM* output =
               noia_drm_update_connector(fd, resources, connector, drm_outputs);
            if (output) {
                noia_list_append(drm_outputs, output);
            }
            drmModeFreeConnector(connector);
        }

        num = noia_list_len(drm_outputs);
        FOR_EACH (drm_outputs, link) {
            noia_list_append(outputs, link->data);
        }

        // Free memory
        drmModeFreeResources(resources);
    }

    noia_list_free(drm_outputs);
    pthread_mutex_unlock(&drm_mutex);
    return num;
}

//------------------------------------------------------------------------------

