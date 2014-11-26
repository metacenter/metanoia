// file: surface-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface.h"
#include "surface-priv.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void aura_surface_commit(SurfaceId id)
{
    SurfaceData* surface = aura_surface_get(id);
    if (surface == 0) {
        LOG_WARN2("Could not find surface (id: %d)", id);
        return;
    }
}

//------------------------------------------------------------------------------

