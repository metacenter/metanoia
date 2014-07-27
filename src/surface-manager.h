// file: surface-manager.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_MANAGER_H__
#define __AURA_SURFACE_MANAGER_H__

#include "global-types.h"

void aura_update_outputs(void);

void aura_surface_subscribe_frame(SurfaceId id, void* notify_data);
void aura_surface_notify_frame(void);

#endif // __AURA_SURFACE_MANAGER_H__

