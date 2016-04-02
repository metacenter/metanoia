// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_STRATEGIST_H
#define NOIA_EXHIBITOR_STRATEGIST_H

#include "exhibitor.h"
#include "exhibitor-compositor.h"

typedef void (*NoiaStrategistOnSurfaceReadyFunc)
             (NoiaExhibitor*, NoiaCompositor*, NoiaSurfaceId);
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

