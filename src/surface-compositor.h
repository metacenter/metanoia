// file: surface-compositor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_COMPOSITOR_H__
#define __AURA_SURFACE_COMPOSITOR_H__

#include "global-types.h"

void aura_surface_attach(SurfaceId id,
                         int width,
                         int height,
                         int stride,
                         char* data,
                         void* resource);

void aura_surface_commit(SurfaceId id);

#endif // __AURA_SURFACE_COMPOSITOR_H__

