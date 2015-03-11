// file: wayland-protocol-keyboard.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-keyboard.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_keyboard_release(NOIA_UNUSED struct wl_client* client,
                                   struct wl_resource* resource)
{
    LOG_WAYL2("Wayland: keyboard release");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

const struct wl_keyboard_interface keyboard_implementation = {
        noia_wayland_keyboard_release
    };

//------------------------------------------------------------------------------

