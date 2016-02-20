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

void noia_wayland_seat_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind seat");
}

//------------------------------------------------------------------------------

void noia_wayland_seat_get_pointer(struct wl_client* client,
                                   struct wl_resource* resource,
                                   uint32_t id)
{
    uint32_t version = wl_resource_get_version(resource);
    noia_wayland_pointer_bind(client, NULL, version, id);
}

//------------------------------------------------------------------------------

void noia_wayland_seat_get_keyboard(struct wl_client* client,
                                    struct wl_resource* resource,
                                    uint32_t id)
{
    uint32_t version = wl_resource_get_version(resource);
    noia_wayland_keyboard_bind(client, NULL, version, id);
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: seat get touch.
void noia_wayland_seat_get_touch(struct wl_client* client     NOIA_UNUSED,
                                 struct wl_resource* resource NOIA_UNUSED,
                                 uint32_t id)
{
    LOG_NYIMP("Wayland > get touch (id: %d)", id);
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: seat release.
void noia_wayland_seat_release(struct wl_client* client     NOIA_UNUSED,
                               struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland > seat release");
}

//------------------------------------------------------------------------------

static const struct wl_seat_interface scSeatImplementation = {
        noia_wayland_seat_get_pointer,
        noia_wayland_seat_get_keyboard,
        noia_wayland_seat_get_touch,
        noia_wayland_seat_release,
    };

//------------------------------------------------------------------------------

void noia_wayland_seat_bind(struct wl_client* client,
                            void* data,
                            uint32_t version,
                            uint32_t id)
{
    LOG_WAYL2("Binding Wayland seat (version: %d, id: %d)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_seat_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scSeatImplementation,
                                   data, noia_wayland_seat_unbind);

    /// @todo Add more capabilities
    wl_seat_send_capabilities(rc, WL_SEAT_CAPABILITY_POINTER
                                | WL_SEAT_CAPABILITY_KEYBOARD);

    if (version >= WL_SEAT_NAME_SINCE_VERSION) {
        wl_seat_send_name(rc, "seat0");
    }
}

//------------------------------------------------------------------------------

