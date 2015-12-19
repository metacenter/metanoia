// file: wayland-protocol-xdg-shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-xdg-shell.h"
#include "wayland-protocol-xdg-surface.h"
#include "wayland-cache.h"

#include "utils-log.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

void noia_wayland_xdg_shell_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind XDG shell");
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_destroy(struct wl_client* client NOIA_UNUSED,
                              struct wl_resource* resource)
{
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_use_unstable_version
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
                                       int32_t version)
{
    LOG_NYIMP("Wayland: use unstable version (version: %d)", version);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_get_xdg_surface(struct wl_client* client,
                                      struct wl_resource* resource,
                                      uint32_t id,
                                      struct wl_resource* surface_resource)
{
    void* data = wl_resource_get_user_data(surface_resource);
    uint32_t version = wl_resource_get_version(resource);
    noia_wayland_xdg_surface_bind(client, data, version, id);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_get_xdg_popup
                              (struct wl_client* client             NOIA_UNUSED,
                               struct wl_resource* resource         NOIA_UNUSED,
                               uint32_t id,
                               struct wl_resource* surface_resource NOIA_UNUSED,
                               struct wl_resource* parent_resource  NOIA_UNUSED,
                               struct wl_resource* seat_resource    NOIA_UNUSED,
                               uint32_t serial,
                               int32_t x,
                               int32_t y)
{
    LOG_NYIMP("Wayland: XDG popup "
              "(id: %d, serial: %d, x: %d, y: %d)",
              id, serial, x, y);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_pong(struct wl_client* client     NOIA_UNUSED,
                           struct wl_resource* resource NOIA_UNUSED,
                           uint32_t serial)
{
    LOG_NYIMP("Wayland: XDG pong (serial: %d)", serial);
}

//------------------------------------------------------------------------------

static const struct xdg_shell_interface scXdgShellImplementation = {
        noia_wayland_xdg_destroy,
        noia_wayland_xdg_use_unstable_version,
        noia_wayland_xdg_get_xdg_surface,
        noia_wayland_xdg_get_xdg_popup,
        noia_wayland_xdg_pong
    };

//------------------------------------------------------------------------------

void noia_wayland_xdg_shell_bind(struct wl_client* client,
                                 void* data,
                                 uint32_t version,
                                 uint32_t id)
{
    LOG_WAYL2("Binding XDG shell (version: %u, id: %u)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &xdg_shell_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    noia_wayland_cache_add_general_resource(NOIA_RESOURCE_OTHER, rc);

    wl_resource_set_implementation(rc, &scXdgShellImplementation,
                                   data, noia_wayland_xdg_shell_unbind);
}

//------------------------------------------------------------------------------

