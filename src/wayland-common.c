// file: wayland-common.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-common.h"

#include "malloc.h"
#include "memory.h"

//------------------------------------------------------------------------------

AuraSurfaceWaylandData* wayland_surface_data_new()
{
    AuraSurfaceWaylandData* data = malloc(sizeof(AuraSurfaceWaylandData));
    if (!data) {
        return 0;
    }

    memset(data, 0, sizeof(AuraSurfaceWaylandData));
    return data;
}

//------------------------------------------------------------------------------

AuraStore* wayland_get_surface_store()
{
    static AuraStore* store = 0;
    if (!store) {
        store = aura_store_new();
    }
    return store;
}

//------------------------------------------------------------------------------

