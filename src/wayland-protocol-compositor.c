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
#include "global-macros.h"

//------------------------------------------------------------------------------

/// @todo: Handle destruction of compositor resource.
void noia_wayland_compositor_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind compositor");
}

//------------------------------------------------------------------------------

/// @todo: Handle destruction of region resource.
void noia_wayland_region_unbind(struct wl_resource* resource)
{
    NoiaItemId rid = (NoiaItemId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: unbind region (rid: %d)", rid);
}

//------------------------------------------------------------------------------

/// Handle destruction of surface resource.
void noia_wayland_surface_unbind(struct wl_resource* resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: unbind surface (sid: %d)", sid);
    noia_surface_destroy(sid);
    noia_wayland_state_remove_surface(sid);
}

//------------------------------------------------------------------------------

/// Wayland protocol: create new surface.
void noia_wayland_create_surface(struct wl_client* client,
                                 struct wl_resource* resource,
                                 uint32_t id)
{
    struct wl_resource* rc;
    NoiaSurfaceId new_sid;

    rc = wl_resource_create(client, &wl_surface_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_resource_post_no_memory(resource);
        return;
    }

    new_sid = noia_surface_create();
    LOG_WAYL2("Wayland: create surface (id: %d, sid: %d)", id, new_sid);

    noia_wayland_state_add_surface(new_sid, rc);

    wl_resource_set_implementation(rc, &surface_implementation,
                                  (void*) new_sid, noia_wayland_surface_unbind);
}

//------------------------------------------------------------------------------

/// Wayland protocol: Create new region.
/// @see wayland-region.h
void noia_wayland_create_region(struct wl_client* client,
                                struct wl_resource* resource,
                                uint32_t id)
{
    struct wl_resource* rc;

    rc = wl_resource_create(client, &wl_region_interface,
                            wl_resource_get_version(resource), id);
    if (!rc) {
        wl_resource_post_no_memory(resource);
        return;
    }

    NoiaItemId new_rid = noia_wayland_cache_create_region();
    LOG_WAYL2("Wayland: create region (id: %d, rid: %d)", id, new_rid);

    wl_resource_set_implementation(rc, &region_implementation,
                                   (void*) new_rid, noia_wayland_region_unbind);
}

//------------------------------------------------------------------------------

const struct wl_compositor_interface compositor_implementation = {
        noia_wayland_create_surface,
        noia_wayland_create_region
    };

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for interface to compositor object.
void noia_wayland_compositor_bind(struct wl_client* client,
                                  void* data NOIA_UNUSED,
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

    noia_wayland_cache_add_general_resource(NOIA_RESOURCE_OTHER, rc);

    wl_resource_set_implementation(rc, &compositor_implementation,
                                   NULL, noia_wayland_compositor_unbind);
}

//------------------------------------------------------------------------------

