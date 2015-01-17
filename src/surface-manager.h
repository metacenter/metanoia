// file: surface-manager.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_MANAGER_H__
#define __AURA_SURFACE_MANAGER_H__

#include "event-loop.h"
#include "surface-data.h"

SurfaceId aura_surface_create(void);

void aura_surface_destroy(SurfaceId sid);

AuraSurfaceData* aura_surface_get(SurfaceId sid);

void aura_surface_attach_egl(SurfaceId sid,
                             void* resource);

void aura_surface_commit(SurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* buffer);

int aura_surface_compare(SurfaceId first, SurfaceId second);

#endif // __AURA_SURFACE_MANAGER_H__

