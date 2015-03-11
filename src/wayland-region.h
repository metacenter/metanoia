// file: wayland-region.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_WAYLAND_REGION_H__
#define __NOIA_WAYLAND_REGION_H__

#include "global-types.h"

typedef struct {
    NoiaItem base;
    NoiaPosition pos;
    NoiaSize size;
} NoiaWaylandRegion;

NoiaWaylandRegion* noia_wayland_region_new(void);

void noia_wayland_region_free(NoiaWaylandRegion* self);

void noia_wayland_region_inflate(NoiaWaylandRegion* self,
                                 int x, int y,
                                 int width, int height);

#endif // __NOIA_WAYLAND_REGION_H__

