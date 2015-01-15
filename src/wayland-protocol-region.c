// file: wayland-protocol-region.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-region.h"

#include "utils-log.h"
#include "global-types.h"

//-----------------------------------------------------------------------------

void aura_wayland_region_destroy(AURA_UNUSED struct wl_client* client,
                                 AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: region destroy");
}

//-----------------------------------------------------------------------------

void aura_wayland_region_add(AURA_UNUSED struct wl_client* client,
                             AURA_UNUSED struct wl_resource* resource,
                             int32_t x,
                             int32_t y,
                             int32_t width,
                             int32_t height)
{
    LOG_NYIMP("Wayland: region add (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//-----------------------------------------------------------------------------

void aura_wayland_region_subtract(AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
                                  int32_t x,
                                  int32_t y,
                                  int32_t width,
                                  int32_t height)
{
    LOG_NYIMP("Wayland: region substract (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//-----------------------------------------------------------------------------

const struct wl_region_interface region_implementation = {
        aura_wayland_region_destroy,
        aura_wayland_region_add,
        aura_wayland_region_subtract
    };

//-----------------------------------------------------------------------------

