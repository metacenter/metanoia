// file: wayland-protocol-seat.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-seat.h"
#include "wayland-protocol-keyboard.h"
#include "wayland-state.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

static void get_pointer(struct wl_client *client,
                        struct wl_resource *resource,
                        uint32_t id)
{
    LOG_NYIMP("Wayland: get pointer (id: %d)", id);
}

//------------------------------------------------------------------------------

static void get_keyboard(struct wl_client *client,
                         struct wl_resource *resource,
                         uint32_t id)
{
    struct wl_resource* res;

    LOG_DATA3("Wayland: get keyboard (id: %d)", id);

    res = wl_resource_create(client, &wl_keyboard_interface,
                             wl_resource_get_version(resource), id);
    if (res == NULL) {
        wl_client_post_no_memory(client);
        return;
    }

    wayland_state_add_keyboard_resource(res, resource);

    wl_resource_set_implementation(res, &keyboard_implementation, NULL, NULL);
}

//------------------------------------------------------------------------------

static void get_touch(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t id)
{
    LOG_NYIMP("Wayland: get touch (id: %d)", id);
}

//------------------------------------------------------------------------------

static const struct wl_seat_interface seat_implementation = {
        get_pointer,
        get_keyboard,
        get_touch
    };

//------------------------------------------------------------------------------

void aura_wayland_seat_bind(struct wl_client *client,
                            void *data, uint32_t version, uint32_t id)
{
    struct wl_resource *resource;

    LOG_NYIMP("Binding Wayland seat (id: %d)", id);

    resource = wl_resource_create(client, &wl_seat_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }

    // TODO: pass unbind callback
    wl_resource_set_implementation(resource, &seat_implementation, NULL, NULL);

    // TODO:
    wl_seat_send_capabilities(resource, WL_SEAT_CAPABILITY_KEYBOARD);
    wl_seat_send_name(resource, "seat0");
}

//------------------------------------------------------------------------------

