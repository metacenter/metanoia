// file: wayland-protocol-region.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-region.h"
#include "wayland-cache.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_region_destroy(NOIA_UNUSED struct wl_client* client,
                                 NOIA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: region destroy");
}

//------------------------------------------------------------------------------

void noia_wayland_region_add(NOIA_UNUSED struct wl_client* client,
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

void noia_wayland_region_subtract(NOIA_UNUSED struct wl_client* client,
                                  NOIA_UNUSED struct wl_resource* resource,
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

