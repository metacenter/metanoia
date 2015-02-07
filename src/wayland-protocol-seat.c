// file: wayland-protocol-seat.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-seat.h"
#include "wayland-protocol-pointer.h"
#include "wayland-protocol-keyboard.h"
#include "wayland-state.h"

#include "utils-log.h"
#include "config.h"

//------------------------------------------------------------------------------

void aura_wayland_pointer_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind pointer");
}

//------------------------------------------------------------------------------

void aura_wayland_keyboard_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind keyboard");
}

//------------------------------------------------------------------------------

void aura_wayland_seat_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind seat");
}

//------------------------------------------------------------------------------

void aura_wayland_get_pointer(struct wl_client* client,
                              struct wl_resource* resource,
                              uint32_t id)
{
    struct wl_resource* rc;

    LOG_NYIMP("Wayland: get pointer (id: %d)", id);

    rc = wl_resource_create(client, &wl_pointer_interface,
                            wl_resource_get_version(resource), id);
    if (rc == NULL) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &pointer_implementation,
                                   NULL, aura_wayland_pointer_unbind);
}

//------------------------------------------------------------------------------

void aura_wayland_get_keyboard(struct wl_client* client,
                               struct wl_resource* resource,
                               uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Wayland: get keyboard (id: %d)", id);

    rc = wl_resource_create(client, &wl_keyboard_interface,
                            wl_resource_get_version(resource), id);
    if (rc == NULL) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &keyboard_implementation,
                                   NULL, aura_wayland_keyboard_unbind);

    // Store resource
    aura_wayland_state_add_keyboard_resource(rc);

    // Send keymap to client
    AuraKeymap* keymap = aura_config_get_keymap();
    if (!keymap) {
        return;
    }

    LOG_WAYL2("Wayland: keyboard map send (format: %d, fd: %d, size: %d)",
              keymap->format, keymap->keymap_fd, keymap->keymap_size);
    wl_keyboard_send_keymap(rc, (uint32_t) keymap->format,
                                (uint32_t) keymap->keymap_fd,
                                (uint32_t) keymap->keymap_size);
}

//------------------------------------------------------------------------------

void aura_wayland_get_touch(AURA_UNUSED struct wl_client* client,
                            AURA_UNUSED struct wl_resource* resource,
                            uint32_t id)
{
    LOG_NYIMP("Wayland: get touch (id: %d)", id);
}

//------------------------------------------------------------------------------

static const struct wl_seat_interface seat_implementation = {
        aura_wayland_get_pointer,
        aura_wayland_get_keyboard,
        aura_wayland_get_touch
    };

//------------------------------------------------------------------------------

void aura_wayland_seat_bind(struct wl_client* client,
                            AURA_UNUSED void* data,
                            uint32_t version,
                            uint32_t id)
{
    struct wl_resource* rc;

    LOG_NYIMP("Binding Wayland seat (version: %d, id: %d)", version, id);

    rc = wl_resource_create(client, &wl_seat_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &seat_implementation,
                                   NULL, aura_wayland_seat_unbind);

    // TODO:
    wl_seat_send_capabilities(rc, WL_SEAT_CAPABILITY_POINTER
                                | WL_SEAT_CAPABILITY_KEYBOARD);
    wl_seat_send_name(rc, "seat0");
}

//------------------------------------------------------------------------------

