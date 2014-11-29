// file: wayland-common.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_COMMON_H__
#define __AURA_WAYLAND_COMMON_H__

#include "utils-store.h"

#include <wayland-server.h>

typedef struct AuraSurfaceWaylandData {
    AuraItem base;
    struct wl_resource* resource;
} AuraSurfaceWaylandData;

AuraSurfaceWaylandData* wayland_surface_data_new();

AuraStore* wayland_get_surface_store();

#endif // __AURA_WAYLAND_COMMON_H__

