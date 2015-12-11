// file: wayland-protocol-seat.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-seat.h"
#include "wayland-protocol-pointer.h"
#include "wayland-protocol-keyboard.h"
#include "wayland-cache.h"
#include "wayland-state.h"

#include "utils-log.h"
#include "config.h"

//------------------------------------------------------------------------------

void noia_wayland_pointer_unbind(struct wl_resource* resource)
{
    LOG_WAYL3("Wayland: unbind pointer");
    noia_wayland_cache_remove_general_resource(NOIA_RESOURCE_POINTER, resource);
}

//------------------------------------------------------------------------------

void noia_wayland_keyboard_unbind(struct wl_resource* resource)
{
    LOG_WAYL3("Wayland: unbind keyboard");
    noia_wayland_cache_remove_general_resource(NOIA_RESOURCE_KEYBOARD,resource);
}

//------------------------------------------------------------------------------

void noia_wayland_seat_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind seat");
}

//------------------------------------------------------------------------------

void noia_wayland_get_pointer(struct wl_client* client,
                              struct wl_resource* resource,
                              uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Wayland: get pointer (id: %d)", id);

    rc = wl_resource_create(client, &wl_pointer_interface,
                            wl_resource_get_version(resource), id);
    if (rc == NULL) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &pointer_implementation,
                                   NULL, noia_wayland_pointer_unbind);

    noia_wayland_cache_add_general_resource(NOIA_RESOURCE_POINTER, rc);
}

//------------------------------------------------------------------------------

void noia_wayland_get_keyboard(struct wl_client* client,
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
                                   NULL, noia_wayland_keyboard_unbind);

    // Store resource
    noia_wayland_state_add_keyboard_resource(rc);

    // Send keymap to client
    NoiaKeymap* keymap = noia_config_get_keymap();
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

void noia_wayland_get_touch(struct wl_client* client     NOIA_UNUSED,
                            struct wl_resource* resource NOIA_UNUSED,
                            uint32_t id)
{
    LOG_NYIMP("Wayland: get touch (id: %d)", id);
}

//------------------------------------------------------------------------------

static const struct wl_seat_interface seat_implementation = {
        noia_wayland_get_pointer,
        noia_wayland_get_keyboard,
        noia_wayland_get_touch
    };

//------------------------------------------------------------------------------

void noia_wayland_seat_bind(struct wl_client* client,
                            void* data NOIA_UNUSED,
                            uint32_t version,
                            uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Binding Wayland seat (version: %d, id: %d)", version, id);

    rc = wl_resource_create(client, &wl_seat_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &seat_implementation,
                                   NULL, noia_wayland_seat_unbind);

    /// @todo Add more capabilities
    wl_seat_send_capabilities(rc, WL_SEAT_CAPABILITY_POINTER
                                | WL_SEAT_CAPABILITY_KEYBOARD);

    if (version >= WL_SEAT_NAME_SINCE_VERSION) {
        wl_seat_send_name(rc, "seat0");
    }
}

//------------------------------------------------------------------------------

