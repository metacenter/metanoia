// file: wayland-protocol-region.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-region.h"
#include "wayland-cache.h"
#include "utils-log.h"

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
    LOG_WAYL2("Wayland: region substract (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//------------------------------------------------------------------------------

const struct wl_region_interface region_implementation = {
        noia_wayland_region_destroy,
        noia_wayland_region_add,
        noia_wayland_region_subtract
    };

//------------------------------------------------------------------------------

