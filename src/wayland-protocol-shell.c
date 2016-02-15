// file: wayland-protocol-shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-shell.h"
#include "wayland-protocol-shell-surface.h"

#include "surface-manager.h"
#include "global-macros.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_shell_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind shell");
}

//------------------------------------------------------------------------------

void noia_wayland_get_shell_surface(struct wl_client* client,
                                    struct wl_resource* resource,
                                    uint32_t id,
                                    struct wl_resource* surface_resource)
{
    void* data = wl_resource_get_user_data(surface_resource);
    uint32_t version = wl_resource_get_version(resource);
    noia_wayland_shell_surface_bind(client, data, version, id);
}

//------------------------------------------------------------------------------

static const struct wl_shell_interface scShellImplementation = {
        noia_wayland_get_shell_surface
    };

//------------------------------------------------------------------------------

void noia_wayland_shell_bind(struct wl_client* client,
                             void* data,
                             uint32_t version,
                             uint32_t id)
{
    LOG_WAYL2("Binding Wayland shell (version: %u, id: %u)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_shell_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scShellImplementation,
                                   data, noia_wayland_shell_unbind);
}

//------------------------------------------------------------------------------

