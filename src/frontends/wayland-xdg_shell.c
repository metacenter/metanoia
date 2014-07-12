// file: wayland-xdg_shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "frontends/wayland-xdg_shell.h"
#include "frontends/wayland-shell-surface.h"

#include "utils/log.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

static void xdg_destroy_shell_surface(struct wl_resource *resource)
{
    LOG_NYIMP("Wayland: destroy shell surface");
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
    struct wl_resource* res;

    LOG_NYIMP("Wayland: get XDG surface (id: %d)", id);

    res = wl_resource_create(client, &xdg_surface_interface, 1, id);
    if (!res) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(res, &shell_surface_implementation,
                                   NULL, xdg_destroy_shell_surface);
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
    struct wl_resource *resource;

    resource = wl_resource_create(client, &xdg_shell_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(resource, &xdg_shell_implementation,
                                   NULL, NULL);
}

//------------------------------------------------------------------------------

