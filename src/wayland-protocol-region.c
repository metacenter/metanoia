// file: wayland-protocol-region.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-region.h"

#include "utils-log.h"
#include "global-types.h"

//-----------------------------------------------------------------------------

static void region_destroy(struct wl_client* client,
                           struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: region destroy");
}

//-----------------------------------------------------------------------------

static void region_add(struct wl_client *client,
                       struct wl_resource *resource,
                       int32_t x,
                       int32_t y,
                       int32_t width,
                       int32_t height)
{
    LOG_NYIMP("Wayland: region add (x: %d, y: %d, w: %d, h: %d)",
              x, y, width, height);
}

//-----------------------------------------------------------------------------

static void region_subtract(struct wl_client *client,
                            struct wl_resource *resource,
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
        region_destroy,
        region_add,
        region_subtract
    };

//-----------------------------------------------------------------------------

