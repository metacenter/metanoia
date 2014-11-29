// file: wayland-protocol-keyboard.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-keyboard.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

static void keyboard_release(struct wl_client *client,
                             struct wl_resource *resource)
{
    LOG_INFO3("Wayland: keyboard release");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

static const struct wl_keyboard_interface keyboard_implementation = {
        keyboard_release
    };

//------------------------------------------------------------------------------

