// file: wayland-protocol-region.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-region.h"
#include "wayland-cache.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void aura_wayland_region_destroy(AURA_UNUSED struct wl_client* client,
                                 AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: region destroy");
}

//------------------------------------------------------------------------------

void aura_wayland_region_add(AURA_UNUSED struct wl_client* client,
                             struct wl_resource* resource,
                             int32_t x,
                             int32_t y,
                             int32_t width,
                             int32_t height)
{
    AuraItemId rid = (AuraItemId) wl_resource_get_user_data(resource);

    LOG_WAYL2("Wayland: region add (rid: %d, x: %d, y: %d, w: %d, h: %d)",
              rid, x, y, width, height);

    AuraWaylandRegion* region = aura_wayland_cache_find_region(rid);
    aura_wayland_region_inflate(region, x, y, width, height);
}

//------------------------------------------------------------------------------

void aura_wayland_region_subtract(AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
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
        aura_wayland_region_destroy,
        aura_wayland_region_add,
        aura_wayland_region_subtract
    };

//------------------------------------------------------------------------------

