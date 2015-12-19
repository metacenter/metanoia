// file: wayland-protocol-keyboard.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-keyboard.h"
#include "wayland-state.h"
#include "wayland-cache.h"

#include "utils-log.h"
#include "global-macros.h"
#include "config.h"

//------------------------------------------------------------------------------

void noia_wayland_keyboard_unbind(struct wl_resource* resource)
{
    LOG_WAYL3("Wayland: unbind keyboard");
    noia_wayland_cache_remove_general_resource(NOIA_RESOURCE_KEYBOARD,resource);
}

//------------------------------------------------------------------------------

void noia_wayland_keyboard_release(struct wl_client* client NOIA_UNUSED,
                                   struct wl_resource* resource)
{
    LOG_WAYL2("Wayland: keyboard release");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

const struct wl_keyboard_interface scKeyboardImplementation = {
        noia_wayland_keyboard_release
    };

//------------------------------------------------------------------------------

void noia_wayland_keyboard_bind(struct wl_client* client,
                                void* data,
                                uint32_t version,
                                uint32_t id)
{
    LOG_WAYL2("Binding Wayland keyboard (version: %d, id: %d)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_keyboard_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scKeyboardImplementation,
                                   data, noia_wayland_keyboard_unbind);

    // Store resource
    noia_wayland_state_add_keyboard_resource(rc);

    // Send keymap to client
    NoiaKeymap* keymap = noia_config_get_keymap();
    NOIA_ENSURE(keymap, return);
    LOG_WAYL2("Wayland: keyboard map send (format: %d, fd: %d, size: %d)",
              keymap->format, keymap->keymap_fd, keymap->keymap_size);
    wl_keyboard_send_keymap(rc, (uint32_t) keymap->format,
                                (uint32_t) keymap->keymap_fd,
                                (uint32_t) keymap->keymap_size);
}

//------------------------------------------------------------------------------

