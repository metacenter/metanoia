// file: wayland-protocol-keyboard.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-keyboard.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

void aura_wayland_pointer_set_cursor(
                               AURA_UNUSED struct wl_client* client,
                               AURA_UNUSED struct wl_resource* resource,
                               uint32_t serial,
                               AURA_UNUSED struct wl_resource* surface_resource,
                               int32_t hotspot_x,
                               int32_t hotspot_y)
{
    LOG_NYIMP("Wayland: set cursor (serial: %d, hotspot_x: %d, hotspot_y: %d)",
              serial, hotspot_x, hotspot_y);
}

//------------------------------------------------------------------------------

void aura_wayland_pointer_release(AURA_UNUSED struct wl_client* client,
                                  struct wl_resource* resource)
{
    LOG_WAYL2("Wayland: pointer release");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

const struct wl_pointer_interface pointer_implementation = {
        aura_wayland_pointer_set_cursor,
        aura_wayland_pointer_release
    };

//------------------------------------------------------------------------------

