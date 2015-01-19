// file: wayland-protocol-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-surface.h"
#include "wayland-state.h"

#include "surface-manager.h"
#include "utils-log.h"

//-----------------------------------------------------------------------------

void aura_wayland_surface_destroy(AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: surface destroy");
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_attach(AURA_UNUSED struct wl_client* client,
                                 struct wl_resource* resource,
                                 struct wl_resource* buffer_resource,
                                 int32_t sx, int32_t sy)
{
    int width = 0;
    int height = 0;
    int stride = 0;
    uint8_t* data = NULL;
    SurfaceId sid = (SurfaceId) wl_resource_get_user_data(resource);

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

    wayland_state_surface_attach(sid, buffer_resource);

    if (!data) {
        aura_surface_attach_egl(sid, buffer_resource);
    } else {
        // TODO: do this on commit
        aura_surface_commit(sid, width, height, stride, data);
    }
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_damage(AURA_UNUSED struct wl_client* client,
                                 AURA_UNUSED struct wl_resource* resource,
                                 int32_t x, int32_t y,
                                 int32_t width, int32_t height)
{
    LOG_NYIMP("Wayland: surface damage (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_frame(struct wl_client* client,
                                struct wl_resource* resource,
                                uint32_t callback)
{
    // TODO: subscribe for destroy

    SurfaceId sid = (SurfaceId) wl_resource_get_user_data(resource);

    LOG_NYIMP("Wayland: surface frame (cb: %d, sid: %d)", callback, sid);

    struct wl_resource* rc = wl_resource_create(client,
                                                &wl_callback_interface,
                                                1, callback);
    if (rc == NULL) {
        LOG_ERROR("Could not create frame callback resource!");
        wl_resource_post_no_memory(resource);
        return;
    }

    // TODO: listen for destroy callback
    wl_resource_set_implementation(rc, NULL, NULL, NULL);

    wayland_state_subscribe_frame(sid, rc);
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_set_opaque_region
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* region_resource)
{
    LOG_NYIMP("Wayland: set opaque region");
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_set_input_region
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* region_resource)
{
    LOG_NYIMP("Wayland: set input region");
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_commit(AURA_UNUSED struct wl_client* client,
                                 struct wl_resource* resource)
{
    SurfaceId sid = (SurfaceId) wl_resource_get_user_data(resource);

    LOG_WAYL3("Wayland: commit (sid: %d)", sid);

    //aura_surface_commit(sid);
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_set_buffer_transform
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       int32_t transform)
{
    LOG_NYIMP("Wayland: set buffer transform (transform: %d)", transform);
}

//-----------------------------------------------------------------------------

void aura_wayland_surface_set_buffer_scale
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       int32_t scale)
{
    LOG_NYIMP("Wayland: set buffer scale (scale: %d)", scale);
}

//-----------------------------------------------------------------------------

const struct wl_surface_interface surface_implementation = {
        aura_wayland_surface_destroy,
        aura_wayland_surface_attach,
        aura_wayland_surface_damage,
        aura_wayland_surface_frame,
        aura_wayland_surface_set_opaque_region,
        aura_wayland_surface_set_input_region,
        aura_wayland_surface_commit,
        aura_wayland_surface_set_buffer_transform,
        aura_wayland_surface_set_buffer_scale
    };

//-----------------------------------------------------------------------------

