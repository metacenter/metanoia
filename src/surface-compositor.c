// file: surface-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface.h"
#include "surface-priv.h"

#include "utils/log.h"

// FIXME: tmp
#include "devices/devfb.h"

//------------------------------------------------------------------------------

void aura_surface_attach(SurfaceId id,
                         int width,
                         int height,
                         int stride,
                         char* data)
{
    SurfaceData* surface = aura_surface_get(id);
    if (surface == 0) {
        LOG_WARN2("Could not find surface (id: %d)", id);
        return;
    }

    surface->pending.width  = width;
    surface->pending.height = height;
    surface->pending.stride = stride;
    surface->pending.data   = data;
}

//------------------------------------------------------------------------------

void aura_surface_commit(SurfaceId id)
{
    SurfaceData* surface = aura_surface_get(id);
    if (surface == 0) {
        LOG_WARN2("Could not find surface (id: %d)", id);
        return;
    }

    aura_surface_manager_redraw_all();

    // TODO: remove
    /*char* data = surface->pending.data;
    int stride = surface->pending.stride;
    int x, y;
    for (y = 0; y < surface->pending.height; ++y) {
        for (x = 0; x < surface->pending.width; ++x) {
            buffer[y*5632 + 4*x + 0] = data[y*stride + 4*x + 0];
            buffer[y*5632 + 4*x + 1] = data[y*stride + 4*x + 1];
            buffer[y*5632 + 4*x + 2] = data[y*stride + 4*x + 2];
            buffer[y*5632 + 4*x + 3] = data[y*stride + 4*x + 3];
        }
    }*/
}

//------------------------------------------------------------------------------

