// file: exhibitor-strategist.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_STRATEGIST_H__
#define __NOIA_EXHIBITOR_STRATEGIST_H__

#include "exhibitor.h"

typedef void (*NoiaStrategistOnSurfaceReadyFunc)
             (NoiaExhibitor*, NoiaSurfaceId);
typedef void (*NoiaStrategistOnSurfaceDestroyedFunc)
             (NoiaExhibitor*, NoiaSurfaceId);

typedef struct {
    NoiaStrategistOnSurfaceReadyFunc     on_surface_ready;
    NoiaStrategistOnSurfaceDestroyedFunc on_surface_destroyed;
} NoiaStrategist;

NoiaStrategist* noia_strategist_new();
void noia_strategist_free(NoiaStrategist* self);

void noia_strategist_setup
                    (NoiaStrategist* self,
                     NoiaStrategistOnSurfaceReadyFunc     on_surface_ready,
                     NoiaStrategistOnSurfaceDestroyedFunc on_surface_destroyed);


#endif // __NOIA_EXHIBITOR_STRATEGIST_H__

