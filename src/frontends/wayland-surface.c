// file: wayland-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "frontends/wayland-surface.h"

#include "utils/log.h"
#include "global-types.h"

// FIXME: tmp
#include "devices/devfb.h"

//-----------------------------------------------------------------------------

static void surface_destroy(struct wl_client* client,
                            struct wl_resource* resource)
{
    LOG_DEBUG("Wayland: surface destroy");
}

//-----------------------------------------------------------------------------

static void surface_attach(struct wl_client* client,
                           struct wl_resource* resource,
                           struct wl_resource* buffer_resource,
                           int32_t sx, int32_t sy)
{
    SurfaceId id = (SurfaceId) wl_resource_get_user_data(resource);

    LOG_DATA3("Wayland: surface attach (sx: %d, sy: %d, id: %d)", sx, sy, id);

    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer_resource);

    int width = wl_shm_buffer_get_width(shm_buffer);
    int height = wl_shm_buffer_get_height(shm_buffer);
    int stride = wl_shm_buffer_get_stride(shm_buffer);
    char* data = wl_shm_buffer_get_data(shm_buffer);


    int x, y;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            buffer[y*5632 + 4*x + 0] = data[y*stride + 4*x + 0];
            buffer[y*5632 + 4*x + 1] = data[y*stride + 4*x + 1];
            buffer[y*5632 + 4*x + 2] = data[y*stride + 4*x + 2];
            buffer[y*5632 + 4*x + 3] = data[y*stride + 4*x + 3];
        }
    }
}

//-----------------------------------------------------------------------------

static void surface_damage(struct wl_client* client,
                           struct wl_resource* resource,
                           int32_t x, int32_t y,
                           int32_t width, int32_t height)
{
    LOG_DEBUG("Wayland: surface damage (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//-----------------------------------------------------------------------------

static void surface_frame(struct wl_client* client,
                          struct wl_resource* resource,
                          uint32_t callback)
{
    LOG_DEBUG("Wayland: surface frame (cb: %d)", callback);
}

//-----------------------------------------------------------------------------

static void surface_set_opaque_region(struct wl_client* client,
                                      struct wl_resource* resource,
                                      struct wl_resource* region_resource)
{
    LOG_DEBUG("Wayland: set opaque region");
}

//-----------------------------------------------------------------------------

static void surface_set_input_region(struct wl_client* client,
                                     struct wl_resource* resource,
                                     struct wl_resource* region_resource)
{
    LOG_DEBUG("Wayland: set input region");
}

//-----------------------------------------------------------------------------

static void surface_commit(struct wl_client* client,
                           struct wl_resource* resource)
{
    LOG_DEBUG("Wayland: commit");
}

//-----------------------------------------------------------------------------

static void surface_set_buffer_transform(struct wl_client* client,
                                         struct wl_resource* resource,
                                         int transform)
{
    LOG_DEBUG("Wayland: set buffer transform");
}

//-----------------------------------------------------------------------------

static void surface_set_buffer_scale(struct wl_client* client,
                                     struct wl_resource* resource,
                                     int32_t scale)
{
    LOG_DEBUG("Wayland: set buffer scale");
}

//-----------------------------------------------------------------------------

const struct wl_surface_interface surface_implementation = {
        surface_destroy,
        surface_attach,
        surface_damage,
        surface_frame,
        surface_set_opaque_region,
        surface_set_input_region,
        surface_commit,
        surface_set_buffer_transform,
        surface_set_buffer_scale
    };

//-----------------------------------------------------------------------------

