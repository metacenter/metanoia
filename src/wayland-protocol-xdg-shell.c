// file: wayland-protocol-xdg-shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-xdg-shell.h"
#include "wayland-protocol-shell-surface.h"

#include "utils-log.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

static void xdg_surface_unbind(struct wl_resource *resource)
{
    LOG_NYIMP("Wayland: unbind XDG shell surface");
}

//------------------------------------------------------------------------------

static void xdg_shell_unbind(struct wl_resource *resource)
{
    LOG_NYIMP("Wayland: unbind XDG shell");
}

//------------------------------------------------------------------------------

static void xdg_use_unstable_version(struct wl_client *client,
                                     struct wl_resource *resource,
                                     int32_t version)
{
    LOG_NYIMP("Wayland: use unstable version");
}

//------------------------------------------------------------------------------

static void xdg_get_xdg_surface(struct wl_client *client,
                                struct wl_resource *resource,
                                uint32_t id,
                                struct wl_resource *surface_resource)
{
    struct wl_resource* rc;

    LOG_NYIMP("Wayland: get XDG surface (id: %d)", id);

    rc = wl_resource_create(client, &xdg_surface_interface, 1, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &shell_surface_implementation,
                                   NULL, xdg_surface_unbind);
}

//------------------------------------------------------------------------------

static void xdg_get_xdg_popup(struct wl_client *client,
                              struct wl_resource *resource,
                              uint32_t id,
                              struct wl_resource *surface_resource,
                              struct wl_resource *parent_resource,
                              struct wl_resource *seat_resource,
                              uint32_t serial,
                              int32_t x,
                              int32_t y,
                              uint32_t flags)
{
    LOG_NYIMP("Wayland: XDG popup");
}

//------------------------------------------------------------------------------

static void xdg_pong(struct wl_client *client,
                     struct wl_resource *resource,
                     uint32_t serial)
{
    LOG_NYIMP("Wayland: XDG pong");
}

//------------------------------------------------------------------------------

static const struct xdg_shell_interface xdg_shell_implementation = {
        xdg_use_unstable_version,
        xdg_get_xdg_surface,
        xdg_get_xdg_popup,
        xdg_pong
    };

//------------------------------------------------------------------------------

void aura_wayland_xdg_shell_bind(struct wl_client *client,
                                 void *data, uint32_t version, uint32_t id)
{
    struct wl_resource* rc;

    rc = wl_resource_create(client, &xdg_shell_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &xdg_shell_implementation,
                                   NULL, xdg_shell_unbind);
}

//------------------------------------------------------------------------------

