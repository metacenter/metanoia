// file: wayland-protocol-keyboard.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-keyboard.h"
#include "wayland-state.h"

#include "utils-log.h"
#include "global-macros.h"

//------------------------------------------------------------------------------

void noia_wayland_pointer_set_cursor(
                               struct wl_client* client             NOIA_UNUSED,
                               struct wl_resource* resource         NOIA_UNUSED,
                               uint32_t serial,
                               struct wl_resource* surface_resource NOIA_UNUSED,
                               int32_t hotspot_x,
                               int32_t hotspot_y)
{
    if (!surface_resource) {
        return;
    }

    NoiaSurfaceId sid =
                    (NoiaSurfaceId) wl_resource_get_user_data(surface_resource);

    LOG_WAYL3("Wayland: set cursor "
              "(serial: %d, hotspot_x: %d, hotspot_y: %d, sid: %d)",
              serial, hotspot_x, hotspot_y, sid);

    noia_wayland_state_set_cursor(serial, hotspot_x, hotspot_y, sid);
}

//------------------------------------------------------------------------------

void noia_wayland_pointer_release(struct wl_client* client NOIA_UNUSED,
                                  struct wl_resource* resource)
{
    LOG_WAYL2("Wayland: pointer release");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

const struct wl_pointer_interface pointer_implementation = {
        noia_wayland_pointer_set_cursor,
        noia_wayland_pointer_release
    };

//------------------------------------------------------------------------------

