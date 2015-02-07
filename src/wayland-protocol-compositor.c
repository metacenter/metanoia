// file: wayland-protocol-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-compositor.h"
#include "wayland-protocol-surface.h"
#include "wayland-protocol-region.h"
#include "wayland-cache.h"
#include "wayland-state.h"

#include "surface-manager.h"
#include "utils-log.h"
#include "global-types.h"

//------------------------------------------------------------------------------

void aura_wayland_compositor_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind compositor");
}

//------------------------------------------------------------------------------

void aura_wayland_region_unbind(AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: unbind region");
}

//------------------------------------------------------------------------------

void aura_wayland_surface_unbind(struct wl_resource* resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: unbind surface (sid: %d)", sid);
    aura_surface_destroy(sid);
    aura_wayland_state_remove_surface(sid);
}

//------------------------------------------------------------------------------

void aura_wayland_create_surface(struct wl_client* client,
                                 struct wl_resource* resource,
                                 uint32_t id)
{
    struct wl_resource* rc;
    AuraSurfaceId new_sid;

    LOG_WAYL2("Wayland: create surface (id: %d)", id);

    rc = wl_resource_create(client, &wl_surface_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_resource_post_no_memory(resource);
        return;
    }

    new_sid = aura_surface_create();
    aura_wayland_state_add_surface(new_sid, rc);

    wl_resource_set_implementation(rc, &surface_implementation,
                                  (void*) new_sid, aura_wayland_surface_unbind);
}

//------------------------------------------------------------------------------

void aura_wayland_create_region(struct wl_client* client,
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
                                   NULL, aura_wayland_region_unbind);
}

//------------------------------------------------------------------------------

const struct wl_compositor_interface compositor_implementation = {
        aura_wayland_create_surface,
        aura_wayland_create_region
    };

//------------------------------------------------------------------------------

void aura_wayland_compositor_bind(struct wl_client* client,
                                  AURA_UNUSED void* data,
                                  uint32_t version,
                                  uint32_t id)
{
    struct wl_resource* rc;

    LOG_WAYL2("Binding Wayland compositor (version: %u, id: %u)", version, id);

    rc = wl_resource_create(client, &wl_compositor_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    aura_wayland_cache_add_general_resource(AURA_RESOURCE_OTHER, rc);

    wl_resource_set_implementation(rc, &compositor_implementation,
                                   NULL, aura_wayland_compositor_unbind);
}

//------------------------------------------------------------------------------

