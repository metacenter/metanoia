// file: wayland-region.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_REGION_H__
#define __AURA_WAYLAND_REGION_H__

#include "global-types.h"

typedef struct {
    AuraItem base;
    AuraPosition pos;
    AuraSize size;
} AuraWaylandRegion;

AuraWaylandRegion* aura_wayland_region_new(void);

void aura_wayland_region_free(AuraWaylandRegion* self);

void aura_wayland_region_inflate(AuraWaylandRegion* self,
                                 int x, int y,
                                 int width, int height);

#endif // __AURA_WAYLAND_REGION_H__

