// file: wayland-protocol-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-compositor.h"
#include "wayland-protocol-surface.h"
#include "wayland-protocol-region.h"
#include "wayland-cache.h"

#include "surface-manager.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

/// @todo: Handle destruction of compositor resource.
void noia_wayland_compositor_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind compositor");
}

//------------------------------------------------------------------------------

/// Wayland protocol: create new surface.
void noia_wayland_create_surface(struct wl_client* client,
                                 struct wl_resource* resource,
                                 uint32_t id)
{
    NoiaSurfaceId new_sid = noia_surface_create();
    LOG_WAYL2("Wayland: create surface (id: %d, sid: %d)", id, new_sid);

    int32_t version = wl_resource_get_version(resource);
    noia_wayland_surface_bind(client, (void*) new_sid, version, id);
}

//------------------------------------------------------------------------------

/// Wayland protocol: Create new region.
/// @see wayland-region.h
void noia_wayland_create_region(struct wl_client* client,
                                struct wl_resource* resource,
                                uint32_t id)
{
    NoiaItemId new_rid = noia_wayland_cache_create_region();
    LOG_WAYL2("Wayland: create region (id: %d, rid: %d)", id, new_rid);

    int32_t version = wl_resource_get_version(resource);
    noia_wayland_region_bind(client, (void*) new_rid, version, id);
}

//------------------------------------------------------------------------------

const struct wl_compositor_interface scCompositorImplementation = {
        noia_wayland_create_surface,
        noia_wayland_create_region
    };

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for interface to compositor object.
void noia_wayland_compositor_bind(struct wl_client* client,
                                  void* data,
                                  uint32_t version,
                                  uint32_t id)
{
    LOG_WAYL2("Binding Wayland compositor (version: %u, id: %u)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_compositor_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scCompositorImplementation,
                                   data, noia_wayland_compositor_unbind);

    noia_wayland_cache_add_general_resource(NOIA_RESOURCE_OTHER, rc);
}

//------------------------------------------------------------------------------

