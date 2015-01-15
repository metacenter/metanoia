// file: wayland-protocol-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-compositor.h"
#include "wayland-protocol-surface.h"
#include "wayland-protocol-region.h"
#include "wayland-state.h"

#include "surface-manager.h"
#include "utils-log.h"
#include "global-types.h"

//------------------------------------------------------------------------------

static void compositor_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind compositor");
}

//------------------------------------------------------------------------------

static void region_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind region");
}

//------------------------------------------------------------------------------

static void surface_unbind(struct wl_resource* resource)
{
    SurfaceId sid = (SurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: unbind surface (sid: %d)", sid);
    aura_surface_destroy(sid);
    wayland_state_remove_surface(sid);
}

//------------------------------------------------------------------------------

static void create_surface(struct wl_client* client,
                           struct wl_resource* resource,
                           uint32_t id)
{
    struct wl_resource* rc;
    SurfaceId new_sid;

    LOG_WAYL2("Wayland: create surface (id: %d)", id);

    rc = wl_resource_create(client, &wl_surface_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_resource_post_no_memory(resource);
        return;
    }

    new_sid = aura_surface_create();
    wayland_state_add_surface(new_sid, rc);

    wl_resource_set_implementation(rc, &surface_implementation,
                                   (void*) new_sid, surface_unbind);
}

//------------------------------------------------------------------------------

static void create_region(struct wl_client* client,
                          struct wl_resource* resource,
                          uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Wayland: create region (id: %d)", id);

    rc = wl_resource_create(client, &wl_region_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_resource_post_no_memory(resource);
        return;
    }

    wl_resource_set_implementation(rc, &region_implementation,
                                   NULL, region_unbind);
}

//------------------------------------------------------------------------------

static const struct wl_compositor_interface compositor_implementation = {
        create_surface,
        create_region
    };

//------------------------------------------------------------------------------

void aura_wayland_compositor_bind(struct wl_client* client,
                                  AURA_UNUSED void* data,
                                  uint32_t version,
                                  uint32_t id)
{
    struct wl_resource* rc;

    rc = wl_resource_create(client, &wl_compositor_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, &compositor_implementation,
                                   NULL, compositor_unbind);
}

//------------------------------------------------------------------------------

