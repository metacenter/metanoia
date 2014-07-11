// file: wayland-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "frontends/wayland-compositor.h"
#include "frontends/wayland-surface.h"

#include "utils/log.h"

//------------------------------------------------------------------------------

static void create_surface(struct wl_client *client,
                           struct wl_resource *resource,
                           uint32_t id)
{
    struct wl_resource* res;

    LOG_DEBUG("Wayland: create surface (id: %d)", id);

    res = wl_resource_create(client, &wl_surface_interface,
                             wl_resource_get_version(resource), id);
    if (!res) {
        wl_resource_post_no_memory(resource);
        return;
    }

    wl_resource_set_implementation(res, &surface_implementation,
                                   NULL, NULL);
}

//------------------------------------------------------------------------------

static void create_region(struct wl_client *client,
                          struct wl_resource *resource,
                          uint32_t id)
{
    LOG_DEBUG("Wayland: create region");
}

//------------------------------------------------------------------------------

static const struct wl_compositor_interface compositor_implementation = {
        create_surface,
        create_region
    };

//------------------------------------------------------------------------------

void aura_wayland_compositor_bind(struct wl_client *client,
                                  void *data, uint32_t version, uint32_t id)
{
    struct wl_resource* resource;

    resource = wl_resource_create(client, &wl_compositor_interface,
                                  version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(resource, &compositor_implementation,
                                   NULL, NULL);
}

//------------------------------------------------------------------------------

