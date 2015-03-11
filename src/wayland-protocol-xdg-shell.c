// file: wayland-protocol-xdg-shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-xdg-shell.h"
#include "wayland-protocol-xdg-surface.h"
#include "wayland-cache.h"

#include "surface-manager.h"
#include "utils-log.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_unbind(NOIA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind XDG shell surface");
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_shell_unbind(NOIA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind XDG shell");
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_use_unstable_version
                                      (NOIA_UNUSED struct wl_client* client,
                                       NOIA_UNUSED struct wl_resource* resource,
                                       int32_t version)
{
    LOG_NYIMP("Wayland: use unstable version (version: %d)", version);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_get_xdg_surface
                              (NOIA_UNUSED struct wl_client* client,
                               NOIA_UNUSED struct wl_resource* resource,
                               uint32_t id,
                               NOIA_UNUSED struct wl_resource* surface_resource)
{
    struct wl_resource* rc;
    NoiaSurfaceId sid =
                    (NoiaSurfaceId) wl_resource_get_user_data(surface_resource);

    LOG_NYIMP("Wayland: get XDG surface (id: %d, sid: %d)", id, sid);

    rc = wl_resource_create(client, &xdg_surface_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    noia_wayland_cache_add_surface_resource
                                     (sid, NOIA_RESOURCE_XDG_SHELL_SURFACE, rc);
    noia_surface_show(sid);

    wl_resource_set_implementation(rc, &xdg_surface_implementation, (void*) sid,
                                   noia_wayland_xdg_surface_unbind);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_get_xdg_popup
                              (NOIA_UNUSED struct wl_client* client,
                               NOIA_UNUSED struct wl_resource* resource,
                               uint32_t id,
                               NOIA_UNUSED struct wl_resource* surface_resource,
                               NOIA_UNUSED struct wl_resource* parent_resource,
                               NOIA_UNUSED struct wl_resource* seat_resource,
                               uint32_t serial,
                               int32_t x,
                               int32_t y,
                               uint32_t flags)
{
    LOG_NYIMP("Wayland: XDG popup "
              "(id: %d, serial: %d, x: %d, y: %d, flags: %d)",
              id, serial, x, y, flags);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_pong(NOIA_UNUSED struct wl_client* client,
                           NOIA_UNUSED struct wl_resource* resource,
                           uint32_t serial)
{
    LOG_NYIMP("Wayland: XDG pong (serial: %d)", serial);
}

//------------------------------------------------------------------------------

static const struct xdg_shell_interface xdg_shell_implementation = {
        noia_wayland_xdg_use_unstable_version,
        noia_wayland_xdg_get_xdg_surface,
        noia_wayland_xdg_get_xdg_popup,
        noia_wayland_xdg_pong
    };

//------------------------------------------------------------------------------

void noia_wayland_xdg_shell_bind(struct wl_client* client,
                                 NOIA_UNUSED void* data,
                                 uint32_t version,
                                 uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Binding XDG shell (version: %u, id: %u)", version, id);

    rc = wl_resource_create(client, &xdg_shell_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    noia_wayland_cache_add_general_resource(NOIA_RESOURCE_OTHER, rc);

    wl_resource_set_implementation(rc, &xdg_shell_implementation,
                                   NULL, noia_wayland_xdg_shell_unbind);
}

//------------------------------------------------------------------------------

