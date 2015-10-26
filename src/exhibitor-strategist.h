// file: exhibitor-strategist.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_STRATEGIST_H__
#define __NOIA_EXHIBITOR_STRATEGIST_H__

#include "exhibitor.h"
#include "global-types.h"

typedef void (*NoiaStrategistOnSurfaceCreatedFunc)
             (NoiaExhibitor*, NoiaSurfaceId);
typedef void (*NoiaStrategistOnSurfaceDestroyedFunc)
             (NoiaExhibitor*, NoiaSurfaceId);

typedef struct {
    NoiaStrategistOnSurfaceCreatedFunc   on_surface_ready;
    NoiaStrategistOnSurfaceDestroyedFunc on_surface_destroyed;
} NoiaStrategist;

NoiaStrategist* noia_strategist_new();
void noia_strategist_free(NoiaStrategist* self);

#endif // __NOIA_EXHIBITOR_STRATEGIST_H__

