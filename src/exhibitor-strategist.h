// file: exhibitor-strategist.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_STRATEGIST_H
#define NOIA_EXHIBITOR_STRATEGIST_H

#include "exhibitor.h"

typedef void (*NoiaStrategistOnSurfaceReadyFunc)
             (NoiaExhibitor*, NoiaSurfaceId);
typedef void (*NoiaStrategistOnSurfaceDestroyedFunc)
             (NoiaExhibitor*, NoiaSurfaceId);

typedef struct {
    NoiaStrategistOnSurfaceReadyFunc     on_surface_ready;
    NoiaStrategistOnSurfaceDestroyedFunc on_surface_destroyed;
} NoiaStrategist;

NoiaStrategist* noia_strategist_new(void);
void noia_strategist_free(NoiaStrategist* self);

void noia_strategist_setup
                    (NoiaStrategist* self,
                     NoiaStrategistOnSurfaceReadyFunc     on_surface_ready,
                     NoiaStrategistOnSurfaceDestroyedFunc on_surface_destroyed);


#endif // NOIA_EXHIBITOR_STRATEGIST_H

