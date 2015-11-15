// file: wayland-protocol-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-surface.h"
#include "wayland-cache.h"
#include "wayland-state.h"

#include "surface-manager.h"
#include "utils-log.h"
#include "global-macros.h"

//------------------------------------------------------------------------------

/// Handle destruction of frame resource.
/// Currently nothing to do here.
void noia_wayland_surface_frame_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_WAYL5("Wayland: unbind surface frame");
}

//------------------------------------------------------------------------------

/// Wayland protocol: destroy surface.
void noia_wayland_surface_destroy(struct wl_client* client     NOIA_UNUSED,
                                  struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: surface destroy");
}

//------------------------------------------------------------------------------

/// Wayland protocol: attach surface.
void noia_wayland_surface_attach(struct wl_client* client NOIA_UNUSED,
                                 struct wl_resource* resource,
                                 struct wl_resource* buffer_resource,
                                 int32_t sx, int32_t sy)
{
    int width = 0;
    int height = 0;
    int stride = 0;
    uint8_t* data = NULL;
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);

    LOG_WAYL3("Wayland: surface attach (sx: %d, sy: %d, sid: %d)", sx, sy, sid);

    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(buffer_resource);
    if (shm_buffer) {
        width  = wl_shm_buffer_get_width(shm_buffer);
        height = wl_shm_buffer_get_height(shm_buffer);
        stride = wl_shm_buffer_get_stride(shm_buffer);
        data   = wl_shm_buffer_get_data(shm_buffer);
    } else {
        LOG_WARN3("Wrong shared memory buffer!");
    }

    noia_wayland_state_surface_attach(sid, buffer_resource);

    if (!data) {
        noia_surface_attach_egl(sid, buffer_resource);
    } else {
        /// @todo Do this on commit
        noia_surface_commit(sid, width, height, stride, data);
    }
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: damage surface.
void noia_wayland_surface_damage(struct wl_client* client     NOIA_UNUSED,
                                 struct wl_resource* resource NOIA_UNUSED,
                                 int32_t x, int32_t y,
                                 int32_t width, int32_t height)
{
    LOG_WAYL3("Wayland: surface damage (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//------------------------------------------------------------------------------

/// Wayland protocol: subscribe for frame.
/// Client subscribes for one-shot notification about redraw of its surface.
void noia_wayland_surface_frame(struct wl_client* client,
                                struct wl_resource* resource,
                                uint32_t callback)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);

    LOG_WAYL3("Wayland: surface frame (cb: %d, sid: %d)", callback, sid);

    struct wl_resource* rc = wl_resource_create(client,
                                                &wl_callback_interface,
                                                1, callback);
    if (rc == NULL) {
        LOG_ERROR("Could not create frame callback resource!");
        wl_resource_post_no_memory(resource);
        return;
    }

    wl_resource_set_implementation(rc, NULL, NULL,
                                   noia_wayland_surface_frame_unbind);

    noia_wayland_state_subscribe_frame(sid, rc);
}

//------------------------------------------------------------------------------

/// #todo: Wayland protocol: setsurface opaque region.
void noia_wayland_surface_set_opaque_region
                                          (struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource,
                                           struct wl_resource* region_resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    NoiaItemId rid = (NoiaItemId) wl_resource_get_user_data(region_resource);

    LOG_NYIMP("Wayland: set opaque region (sid: %d, rid: %d)", sid, rid);
}

//------------------------------------------------------------------------------

/// Wayland protocol: set surface input region.
void noia_wayland_surface_set_input_region(struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource,
                                           struct wl_resource* region_resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    NoiaItemId rid = (NoiaItemId) wl_resource_get_user_data(region_resource);

    LOG_WAYL2("Wayland: set input region (sid: %d, rid: %d)", sid, rid);

    NoiaWaylandRegion* region = noia_wayland_cache_find_region(rid);
    if (region) {
        noia_surface_set_offset(sid, region->pos);
        noia_surface_set_requested_size(sid, region->size);
    }
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: commit surface.
/// Client tells compositor that all request were sent and the surface is now
/// ready to draw.
void noia_wayland_surface_commit(struct wl_client* client NOIA_UNUSED,
                                 struct wl_resource* resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);

    LOG_WAYL3("Wayland: commit (sid: %d)", sid);

    //noia_surface_commit(sid);
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: set surface buffer transform.
void noia_wayland_surface_set_buffer_transform
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
                                       int32_t transform)
{
    LOG_NYIMP("Wayland: set buffer transform (transform: %d)", transform);
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: set surface buffer scale.
void noia_wayland_surface_set_buffer_scale
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
                                       int32_t scale)
{
    LOG_NYIMP("Wayland: set buffer scale (scale: %d)", scale);
}

//------------------------------------------------------------------------------

const struct wl_surface_interface surface_implementation = {
        noia_wayland_surface_destroy,
        noia_wayland_surface_attach,
        noia_wayland_surface_damage,
        noia_wayland_surface_frame,
        noia_wayland_surface_set_opaque_region,
        noia_wayland_surface_set_input_region,
        noia_wayland_surface_commit,
        noia_wayland_surface_set_buffer_transform,
        noia_wayland_surface_set_buffer_scale
    };

//------------------------------------------------------------------------------

