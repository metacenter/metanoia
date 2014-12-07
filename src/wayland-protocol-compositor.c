// file: wayland-protocol-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-compositor.h"
#include "wayland-protocol-surface.h"
#include "wayland-protocol-region.h"
#include "wayland-state.h"

#include "surface.h"
#include "utils-log.h"
#include "global-types.h"

//------------------------------------------------------------------------------

static void create_surface(struct wl_client* client,
                           struct wl_resource* resource,
                           uint32_t id)
{
    struct wl_resource* res;
    SurfaceId new_sid;

    LOG_WAYL2("Wayland: create surface (id: %d)", id);

    res = wl_resource_create(client, &wl_surface_interface,
                             wl_resource_get_version(resource), id);
    if (!res) {
        wl_resource_post_no_memory(resource);
        return;
    }

    new_sid = aura_surface_create();
    wayland_state_add_surface(new_sid, res);

    wl_resource_set_implementation(res, &surface_implementation,
                                   (void*) new_sid, NULL);
}

//------------------------------------------------------------------------------

static void create_region(struct wl_client* client,
                          struct wl_resource* resource,
                          uint32_t id)
{
    struct wl_resource* res;

    LOG_WAYL2("Wayland: create region (id: %d)", id);

    res = wl_resource_create(client, &wl_region_interface, 1, id);
    if (!res) {
        wl_resource_post_no_memory(resource);
        return;
    }

    // TODO: add destroy callback
    wl_resource_set_implementation(res, &region_implementation,
                                   NULL, NULL);
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

