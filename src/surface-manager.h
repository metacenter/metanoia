// file: surface-manager.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_MANAGER_H__
#define __AURA_SURFACE_MANAGER_H__

#include "global-types.h"
#include "event-loop.h"

void aura_surface_manager_initialize(AuraLoop* this_loop);

void aura_surface_attach_egl(SurfaceId id,
                             void* resource);

void aura_surface_commit(SurfaceId id,
                         int width,
                         int height,
                         int stride,
                         uint8_t* buffer);

#endif // __AURA_SURFACE_MANAGER_H__

