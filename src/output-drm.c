// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "output-drm.h"

#include "utils-log.h"
#include "renderer-mmap.h"
#include "renderer-gl.h"
#include "output-internal.h"
#include "config.h"

#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <string.h>
#include <pthread.h>
#include <sys/mman.h>

//------------------------------------------------------------------------------

pthread_mutex_t drm_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef uint32_t (*NoiaOutputDRMSwapBuffersFunc) (NoiaOutputDRM*);

struct NoiaOutputDRMStruct {
    NoiaOutput base; ///< Base output.

    // DRM
    int fd;                ///< DRM device file descriptor.
    uint32_t crtc_id;      ///< ID of CRTC.
    uint32_t connector_id; ///< ID of connector.
    drmModeModeInfo mode;  ///< Screen resolution.

    // dumb buffers
    int front;      ///< Front buffer number (0 or 1).
    uint32_t fb[2]; ///< Array for frame buffers.

    // GBM
    struct gbm_surface* gbm_surface; ///< GBM surface.
    struct gbm_bo* gbm_bo;           ///< GBM buffer object.

    // strategies
    NoiaOutputDRMSwapBuffersFunc swap_buffers; ///< Swap buffers strategy.
};

/// Data type used to bind DRM frame buffer information to GBM buffer object.
typedef struct {
    uint32_t fb;
} NoiaGbmFbBundle;

//------------------------------------------------------------------------------
// PRIVATE

/// Set mode using drmModeSetCrtc.
NoiaResult noia_output_drm_set_mode(NoiaOutputDRM* output_drm, int32_t fb)
{
    NoiaResult result = NOIA_RESULT_ERROR;
    NOIA_ENSURE(output_drm, return result);

    pthread_mutex_lock(&drm_mutex);

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

/// Call output DRM strategy for swapping buffers.
/// @see noia_output_drm_swap_dumb_buffers, noia_output_drm_swap_gbm_surfaces
uint32_t noia_output_drm_swap_buffers(NoiaOutputDRM* self)
{
    NOIA_ENSURE(self, return INVALID_FB_ID);
    NOIA_ENSURE(self->swap_buffers, return INVALID_FB_ID);

    return self->swap_buffers(self);
}

//------------------------------------------------------------------------------
// GBM

/// GBM object destruction event handler.
/// Frees memory allocated in `noia_output_drm_gbm_create`.
void noia_output_drm_destroy_gbm_bundle(struct gbm_bo* bo,
                                        void* data NOIA_UNUSED)
{
    LOG_INFO2("Destroying GBM buffer object!");
    NoiaGbmFbBundle* bundle = gbm_bo_get_user_data(bo);
    NOIA_ENSURE(bundle, return);
    free(bundle);
}

//------------------------------------------------------------------------------

/// Create DRM frame buffer for use with GBM.
/// @see NoiaGBMFbBundle
NoiaGbmFbBundle* noia_output_drm_gbm_create(int drm_fd, struct gbm_bo* bo)
{
    NoiaGbmFbBundle* bundle = NULL;
    uint32_t fb = INVALID_FB_ID;
    uint32_t width  = gbm_bo_get_width(bo);
    uint32_t height = gbm_bo_get_height(bo);
    uint32_t stride = gbm_bo_get_stride(bo);
    uint32_t handle = gbm_bo_get_handle(bo).u32;

    if (not drmModeAddFB(drm_fd, width, height, 24, 32, stride, handle, &fb)) {
        bundle = malloc(sizeof(*bundle));
        bundle->fb = fb;
    } else {
        LOG_ERROR("Failed to create DRM framebuffer: %m");
    }

    gbm_bo_set_user_data(bo, bundle, noia_output_drm_destroy_gbm_bundle);
    return bundle;
}

//------------------------------------------------------------------------------

/// Page flip event handler for drmHandleEvent.
void noia_output_drm_handle_page_flip_event(int fd             NOIA_UNUSED,
                                            unsigned int frame NOIA_UNUSED,
                                            unsigned int sec   NOIA_UNUSED,
                                            unsigned int usec  NOIA_UNUSED,
                                            void* data)
{
    NoiaOutput* output = data;
    NOIA_ENSURE(output, return);
    noia_output_notify_vblank(output);
}

//------------------------------------------------------------------------------

/// Handler for DRM device events.
void noia_output_drm_vblank_handler(NoiaEventData* data,
                                    struct epoll_event* epev NOIA_UNUSED)
{
    NoiaOutputDRM* output_drm = noia_event_data_get_data(data);

    drmEventContext ev;
    memset(&ev, 0, sizeof(ev));
    ev.version = DRM_EVENT_CONTEXT_VERSION;
    ev.page_flip_handler = noia_output_drm_handle_page_flip_event;

    drmHandleEvent(output_drm->fd, &ev);
}

//------------------------------------------------------------------------------
// EGL

/// Lock GBM surface and create new frame buffer if needed.
uint32_t noia_output_drm_swap_gbm_surfaces(NoiaOutputDRM* output_drm)
{
    NOIA_ENSURE(output_drm->gbm_surface, return INVALID_FB_ID)

    gbm_surface_release_buffer(output_drm->gbm_surface, output_drm->gbm_bo);

    struct gbm_bo* bo = gbm_surface_lock_front_buffer(output_drm->gbm_surface);
    if (not bo) {
        LOG_ERROR("Could not lock GBM front buffer!");
    } else {
        output_drm->gbm_bo = bo;
    }

    NoiaGbmFbBundle* bundle = gbm_bo_get_user_data(output_drm->gbm_bo);
    if (not bundle) {
        bundle = noia_output_drm_gbm_create(output_drm->fd, output_drm->gbm_bo);
    }

    return bundle->fb;
}

//------------------------------------------------------------------------------

/// Prepare GL render for use with GBM buffers.
NoiaRenderer* noia_output_drm_initialize_egl(NoiaOutputDRM* output_drm)
{
    NoiaEGLBundle egl;
    NoiaGBMBundle gbm;

    LOG_INFO1("Creating GBM and initializing EGL...");

    /// @todo Find mode
    NoiaSize size = {output_drm->mode.hdisplay, output_drm->mode.vdisplay};

    // Initialize GBM
    noia_drm_create_gbm_surface(output_drm->fd, size, &gbm);
    if (gbm.device and gbm.surface) {
        output_drm->gbm_surface = gbm.surface;
    } else {
        return NULL;
    }

    // Initialize EGL
    NoiaResult result = noia_gl_create_onscreen_egl_bundle
                                             ((EGLNativeDisplayType) gbm.device,
                                              (EGLNativeWindowType) gbm.surface,
                                              &egl);
    if (result != NOIA_RESULT_SUCCESS) {
        return NULL;
    }

    LOG_INFO1("Creating GBM and initializing EGL: SUCCESS");

    output_drm->swap_buffers = noia_output_drm_swap_gbm_surfaces;
    return noia_renderer_gl_create(&egl, size);
}

//------------------------------------------------------------------------------
// DUMB BUFFERS

/// Create single DRM dumb buffer for use with renderer.
NoiaResult noia_output_drm_create_dumb_buffer(NoiaOutputDRM* output_drm,
                                              NoiaRenderer* renderer,
                                              int num)
{
    int res;
    uint8_t* map = NULL;
    struct drm_mode_create_dumb carg;
    struct drm_mode_destroy_dumb darg;
    struct drm_mode_map_dumb marg;
    NoiaResult result = NOIA_RESULT_ERROR;
    output_drm->fb[num] = INVALID_FB_ID;

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
            output_drm->fb[num] = INVALID_FB_ID;
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

/// Swap dumb buffers.
/// This function is used only for dumb buffers.
/// @see noia_output_drm_swap_gbm_surfaces
uint32_t noia_output_drm_swap_dumb_buffers(NoiaOutputDRM* output_drm)
{
    output_drm->front = output_drm->front ^ 1;
    return output_drm->fb[output_drm->front];
}

//------------------------------------------------------------------------------

/// Prepare MMap render for use with DRM dumb buffers.
NoiaRenderer* noia_output_drm_create_dumb_buffers(NoiaOutputDRM* output_drm)
{
    NoiaRenderer* renderer = noia_renderer_mmap_create(&output_drm->base);
    noia_output_drm_create_dumb_buffer(output_drm, renderer, 0);
    noia_output_drm_create_dumb_buffer(output_drm, renderer, 1);
    output_drm->swap_buffers = noia_output_drm_swap_dumb_buffers;
    return renderer;
}

//------------------------------------------------------------------------------
// OUTPUT

/// Prepare output for rendering.
/// Depending on hardware and settings create renderer for drawing with GL
/// or on DRM dumb buffers.
NoiaResult noia_output_drm_initialize(NoiaOutput* output,
                                      NoiaSize size NOIA_UNUSED)
{
    NoiaResult result = NOIA_RESULT_ERROR;
    NoiaOutputDRM* output_drm = (NoiaOutputDRM*) output;
    NOIA_ENSURE(output_drm, return result);

    NOIA_BLOCK {
        if (noia_settings()->use_gl) {
            output->renderer = noia_output_drm_initialize_egl(output_drm);
        }

        if (not output->renderer) {
            output->renderer = noia_output_drm_create_dumb_buffers(output_drm);
        }

        if (not output->renderer) {
            LOG_ERROR("DRM failed!");
            break;
        }

        NOIA_ASSERT_RESULT(noia_renderer_initialize(output->renderer));
        noia_renderer_swap_buffers(output->renderer);

        int32_t fb = noia_output_drm_swap_buffers(output_drm);
        NOIA_ASSERT_RESULT(noia_output_drm_set_mode(output_drm, fb));

        result = NOIA_RESULT_SUCCESS;
    }

    if ((result != NOIA_RESULT_SUCCESS) and (output->renderer)) {
        noia_renderer_destroy(output->renderer);
    }

    return result;
}

//------------------------------------------------------------------------------

/// Creates event data for dispatcher to handle DRM device events.
NoiaEventData* noia_output_drm_get_redraw_event(NoiaOutput* output)
{
    NoiaOutputDRM* output_drm = (NoiaOutputDRM*) output;
    NOIA_ENSURE(output_drm, return NULL);

    return noia_event_data_create(output_drm->fd,
                                  output_drm,
                                  noia_output_drm_vblank_handler,
                                  NULL);
}

//------------------------------------------------------------------------------

/// Swaps buffers and schedules page flip on device.
NoiaResult noia_output_drm_schedule_page_flip(NoiaOutput* output)
{
    NoiaOutputDRM* output_drm = (NoiaOutputDRM*) output;
    NOIA_ENSURE(output_drm, return NOIA_RESULT_INCORRECT_ARGUMENT);

    pthread_mutex_lock(&drm_mutex);
    NoiaResult result = NOIA_RESULT_ERROR;

    noia_renderer_swap_buffers(output_drm->base.renderer);

    int32_t fb = noia_output_drm_swap_buffers(output_drm);
    int r = drmModePageFlip(output_drm->fd, output_drm->crtc_id, fb,
                            DRM_MODE_PAGE_FLIP_EVENT, output_drm);
    if (r) {
        LOG_ERROR("Failed to page flip "
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
void noia_output_drm_free(NoiaOutput* output)
{
    NOIA_ENSURE(output, return);
    if (output->unique_name) {
        free(output->unique_name);
    }
    free(output);
}

//------------------------------------------------------------------------------
// PUBLIC

/// DRM output constructor.
NoiaOutputDRM* noia_output_drm_create(NoiaDRMBundle* drm)
{
    NoiaOutputDRM* output_drm = calloc(1, sizeof(NoiaOutputDRM));
    NOIA_ENSURE(output_drm, abort());

    drmModeConnectorPtr connector =
                                drmModeGetConnector(drm->fd, drm->connector_id);
    drmModeModeInfo mode = connector->modes[0];
    NoiaSize size = {mode.hdisplay, mode.vdisplay};

    noia_output_setup(&output_drm->base,
                      size,
                      strdup(noia_drm_get_connector_name(connector)),
                      noia_output_drm_initialize,
                      noia_output_drm_get_redraw_event,
                      noia_output_drm_schedule_page_flip,
                      noia_output_drm_free);

    output_drm->fd = drm->fd;
    output_drm->crtc_id = drm->crtc_id;
    output_drm->connector_id = drm->connector_id;
    output_drm->mode = mode;
    output_drm->front = 0;
    output_drm->fb[0] = INVALID_FB_ID;
    output_drm->fb[1] = INVALID_FB_ID;
    output_drm->gbm_surface = NULL;
    output_drm->gbm_bo = NULL;

    drmModeFreeConnector(connector);

    return output_drm;
}

//------------------------------------------------------------------------------

