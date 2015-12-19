// file: wayland-protocol-region.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-region.h"
#include "wayland-cache.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

/// @todo: Handle destruction of region resource.
void noia_wayland_region_unbind(struct wl_resource* resource)
{
    NoiaItemId rid = (NoiaItemId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: unbind region (rid: %d)", rid);
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: destroy region.
void noia_wayland_region_destroy(struct wl_client* client     NOIA_UNUSED,
                                 struct wl_resource* resource NOIA_UNUSED)
{
    NoiaItemId rid = (NoiaItemId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: region destroy (rid: %d)", rid);
}

//------------------------------------------------------------------------------

/// Wayland protocol: add a square to a region.
/// Here concept is simplified.
/// @see wayland-region.h
void noia_wayland_region_add(struct wl_client* client NOIA_UNUSED,
                             struct wl_resource* resource,
                             int32_t x,
                             int32_t y,
                             int32_t width,
                             int32_t height)
{
    NoiaItemId rid = (NoiaItemId) wl_resource_get_user_data(resource);

    LOG_WAYL2("Wayland: region add (rid: %d, x: %d, y: %d, w: %d, h: %d)",
              rid, x, y, width, height);

    NoiaWaylandRegion* region = noia_wayland_cache_find_region(rid);
    noia_wayland_region_inflate(region, x, y, width, height);
}

//------------------------------------------------------------------------------

/// @todo Wayland protocol: subtract a square from a region.
/// @see wayland-region.h
void noia_wayland_region_subtract(struct wl_client* client     NOIA_UNUSED,
                                  struct wl_resource* resource NOIA_UNUSED,
                                  int32_t x,
                                  int32_t y,
                                  int32_t width,
                                  int32_t height)
{
    LOG_WAYL2("Wayland: region subtract (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//------------------------------------------------------------------------------

const struct wl_region_interface scRegionImplementation = {
        noia_wayland_region_destroy,
        noia_wayland_region_add,
        noia_wayland_region_subtract
    };

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for interface to region object.
void noia_wayland_region_bind(struct wl_client* client,
                              void* data,
                              uint32_t version,
                              uint32_t id)
{
    LOG_WAYL2("Binding Wayland region (version: %u, id: %u)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_region_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scRegionImplementation,
                                   data, noia_wayland_region_unbind);
}

//------------------------------------------------------------------------------

