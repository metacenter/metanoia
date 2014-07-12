// file: wayland-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "frontends/wayland-compositor.h"
#include "frontends/wayland-surface.h"

#include "surface.h"
#include "utils/log.h"
#include "global-types.h"

//------------------------------------------------------------------------------

static void create_surface(struct wl_client* client,
                           struct wl_resource* resource,
                           uint32_t proxy_id)
{
    struct wl_resource* res;
    SurfaceId new_surface_id;

    LOG_DATA3("Wayland: create surface (id: %d)", proxy_id);

    res = wl_resource_create(client, &wl_surface_interface,
                             wl_resource_get_version(resource), proxy_id);
    if (!res) {
        wl_resource_post_no_memory(resource);
        return;
    }

    new_surface_id = aura_surface_create();

    wl_resource_set_implementation(res, &surface_implementation,
                                   (void*) new_surface_id, NULL);
}

//------------------------------------------------------------------------------

static void create_region(struct wl_client* client,
                          struct wl_resource* resource,
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

void aura_wayland_compositor_bind(struct wl_client* client,
                                  void* data, uint32_t version, uint32_t id)
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

