// file: exhibitor-strategist.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_STRATEGIST_H__
#define __AURA_EXHIBITOR_STRATEGIST_H__

#include "exhibitor.h"
#include "global-types.h"

typedef void (*AuraStrategistOnSurfaceCreatedFunc)
             (AuraExhibitor*, AuraSurfaceId);
typedef void (*AuraStrategistOnSurfaceDestroyedFunc)
             (AuraExhibitor*, AuraSurfaceId);

typedef struct {
    AuraStrategistOnSurfaceCreatedFunc   on_surface_ready;
    AuraStrategistOnSurfaceDestroyedFunc on_surface_destroyed;
} AuraStrategist;

AuraStrategist* aura_strategist_create();
void aura_strategist_destroy(AuraStrategist* self);

#endif // __AURA_EXHIBITOR_STRATEGIST_H__
