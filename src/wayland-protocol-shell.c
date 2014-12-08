// file: wayland-protocol-shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-shell.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

static void shell_unbind(struct wl_resource *resource)
{
    LOG_NYIMP("Wayland: unbind shell");
}

//------------------------------------------------------------------------------

static void get_shell_surface(struct wl_client *client,
                              struct wl_resource *resource,
                              uint32_t id,
                              struct wl_resource *surface_resource)
{
    LOG_NYIMP("Getting Wayland shell surface (id: %d)", id);
}

//------------------------------------------------------------------------------

static const struct wl_shell_interface shell_implementation = {
        get_shell_surface
    };

//------------------------------------------------------------------------------

void aura_wayland_shell_bind(struct wl_client *client,
                             void *data, uint32_t version, uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Binding Wayland shell (id: %d)", id);

    rc = wl_resource_create(client, &wl_shell_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &shell_implementation,
                                   NULL, shell_unbind);
}

//------------------------------------------------------------------------------

