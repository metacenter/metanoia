// file: wayland-protocol-keyboard.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-keyboard.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

static void pointer_set_cursor(struct wl_client* client,
                               struct wl_resource* resource,
                               uint32_t serial,
                               struct wl_resource* surface_resource,
                               int32_t hotspot_x,
                               int32_t hotspot_y)
{
    LOG_NYIMP("Wayland: set cursor (serial: %d, hotspot_x: %d, hotspot_y: %d)",
              serial, hotspot_x, hotspot_y);
}

//------------------------------------------------------------------------------

static void pointer_release(struct wl_client *client,
                            struct wl_resource *resource)
{
    LOG_WAYL2("Wayland: pointer release");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

static const struct wl_pointer_interface pointer_implementation = {
        pointer_set_cursor,
        pointer_release
    };

//------------------------------------------------------------------------------

