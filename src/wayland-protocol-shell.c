// file: wayland-protocol-shell.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-shell.h"
#include "wayland-protocol-shell-surface.h"
#include "wayland-cache.h"

#include "surface-manager.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind shell surface");
}

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
    struct wl_resource* rc;
    NoiaSurfaceId sid =
                    (NoiaSurfaceId) wl_resource_get_user_data(surface_resource);

    LOG_NYIMP("Getting Wayland shell surface (id: %32d, sid: %d)", id, sid);

    rc = wl_resource_create(client, &wl_shell_surface_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_resource_post_no_memory(resource);
        return;
    }

    noia_wayland_cache_add_surface_resource
                                  (sid, NOIA_RESOURCE_SHELL_SURFACE, rc);
    noia_surface_show(sid);

    wl_resource_set_implementation(rc, &shell_surface_implementation,
                                   NULL, noia_wayland_shell_surface_unbind);
}

//------------------------------------------------------------------------------

static const struct wl_shell_interface shell_implementation = {
        noia_wayland_get_shell_surface
    };

//------------------------------------------------------------------------------

void noia_wayland_shell_bind(struct wl_client* client,
                             void* data NOIA_UNUSED,
                             uint32_t version,
                             uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Binding Wayland shell (version: %u, id: %u)", version, id);

    rc = wl_resource_create(client, &wl_shell_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &shell_implementation,
                                   NULL, noia_wayland_shell_unbind);
}

//------------------------------------------------------------------------------

