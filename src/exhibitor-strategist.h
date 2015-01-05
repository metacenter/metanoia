// file: exhibitor-strategist.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_STRATEGIST_H__
#define __AURA_EXHIBITOR_STRATEGIST_H__

#include "exhibitor.h"
#include "global-types.h"

typedef void (*AuraStrategistOnSurfaceCreatedFunc) (AuraExhibitor*, SurfaceId);

typedef struct {
    AuraStrategistOnSurfaceCreatedFunc on_surface_created;
} AuraStrategist;

AuraStrategist* aura_strategist_create();

#endif // __AURA_EXHIBITOR_STRATEGIST_H__