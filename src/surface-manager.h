// file: surface-manager.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_MANAGER_H__
#define __AURA_SURFACE_MANAGER_H__

#include "event-loop.h"
#include "surface-data.h"

AuraSurfaceId aura_surface_create(void);

void aura_surface_destroy(AuraSurfaceId sid);

AuraSurfaceData* aura_surface_get(AuraSurfaceId sid);

void aura_surface_attach_egl(AuraSurfaceId sid,
                             void* resource);

void aura_surface_commit(AuraSurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* buffer);

void aura_surface_set_offset(AuraSurfaceId sid, AuraPosition offset);

void aura_surface_set_requested_size(AuraSurfaceId sid,
                                     AuraSize size);

int aura_surface_compare(AuraSurfaceId first, AuraSurfaceId second);

void aura_surface_lock();

void aura_surface_unlock();

void aura_surface_clear_all();

#endif // __AURA_SURFACE_MANAGER_H__

