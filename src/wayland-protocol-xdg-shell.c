// file: wayland-protocol-xdg-shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-xdg-shell.h"
#include "wayland-protocol-xdg-surface.h"
#include "wayland-cache.h"

#include "surface-manager.h"
#include "utils-log.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind XDG shell surface");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_shell_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind XDG shell");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_use_unstable_version
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       int32_t version)
{
    LOG_NYIMP("Wayland: use unstable version (version: %d)", version);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_get_xdg_surface
                              (AURA_UNUSED struct wl_client* client,
                               AURA_UNUSED struct wl_resource* resource,
                               uint32_t id,
                               AURA_UNUSED struct wl_resource* surface_resource)
{
    struct wl_resource* rc;
    AuraSurfaceId sid =
                    (AuraSurfaceId) wl_resource_get_user_data(surface_resource);

    LOG_NYIMP("Wayland: get XDG surface (id: %d, sid: %d)", id, sid);

    rc = wl_resource_create(client, &xdg_surface_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    aura_wayland_cache_add_surface_resource
                                     (sid, AURA_RESOURCE_XDG_SHELL_SURFACE, rc);
    aura_surface_show(sid);

    wl_resource_set_implementation(rc, &xdg_surface_implementation, (void*) sid,
                                   aura_wayland_xdg_surface_unbind);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_get_xdg_popup
                              (AURA_UNUSED struct wl_client* client,
                               AURA_UNUSED struct wl_resource* resource,
                               uint32_t id,
                               AURA_UNUSED struct wl_resource* surface_resource,
                               AURA_UNUSED struct wl_resource* parent_resource,
                               AURA_UNUSED struct wl_resource* seat_resource,
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

void aura_wayland_xdg_pong(AURA_UNUSED struct wl_client* client,
                           AURA_UNUSED struct wl_resource* resource,
                           uint32_t serial)
{
    LOG_NYIMP("Wayland: XDG pong (serial: %d)", serial);
}

//------------------------------------------------------------------------------

static const struct xdg_shell_interface xdg_shell_implementation = {
        aura_wayland_xdg_use_unstable_version,
        aura_wayland_xdg_get_xdg_surface,
        aura_wayland_xdg_get_xdg_popup,
        aura_wayland_xdg_pong
    };

//------------------------------------------------------------------------------

void aura_wayland_xdg_shell_bind(struct wl_client* client,
                                 AURA_UNUSED void* data,
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

    aura_wayland_cache_add_general_resource(AURA_RESOURCE_OTHER, rc);

    wl_resource_set_implementation(rc, &xdg_shell_implementation,
                                   NULL, aura_wayland_xdg_shell_unbind);
}

//------------------------------------------------------------------------------

