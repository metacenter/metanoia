// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_SURFACE_COORDINATOR_H
#define NOIA_SURFACE_COORDINATOR_H

#include "surface-data.h"

typedef struct NoiaCoordinatorStruct NoiaCoordinator;

//------------------------------------------------------------------------------

NoiaCoordinator* noia_coordinator_new();

void noia_coordinator_free(NoiaCoordinator* self);

void noia_coordinator_lock_surfaces(NoiaCoordinator* self);

void noia_coordinator_unlock_surfaces(NoiaCoordinator* self);

void noia_coordinator_notify(NoiaCoordinator* self);

NoiaMilliseconds noia_coordinator_get_last_notify(NoiaCoordinator* self);

//------------------------------------------------------------------------------

NoiaSurfaceId noia_surface_create(NoiaCoordinator* coordinator);

void noia_surface_destroy(NoiaCoordinator* coordinator, NoiaSurfaceId sid);

NoiaSurfaceData* noia_surface_get(NoiaCoordinator* coordinator,
                                  NoiaSurfaceId sid);

void noia_surface_attach_egl(NoiaCoordinator* coordinator,
                             NoiaSurfaceId sid,
                             void* resource);

void noia_surface_attach(NoiaCoordinator* coordinator,
                         NoiaSurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* buffer,
                         void* resource);

void noia_surface_commit(NoiaCoordinator* coordinator, NoiaSurfaceId sid);

void noia_surface_show(NoiaCoordinator* coordinator,
                       NoiaSurfaceId sid,
                       NoiaSurfaceShowReason reason);

void noia_surface_reconfigure(NoiaCoordinator* coordinator,
                              NoiaSurfaceId sid,
                              NoiaSize size,
                              uint8_t state_flags);

void noia_surface_set_focus(NoiaCoordinator* coordinator, NoiaSurfaceId sid);

void noia_surface_set_offset(NoiaCoordinator* coordinator,
                             NoiaSurfaceId sid,
                             NoiaPosition offset);

void noia_surface_set_requested_size(NoiaCoordinator* coordinator,
                                     NoiaSurfaceId sid,
                                     NoiaSize size);

void noia_surface_set_requested_position(NoiaCoordinator* coordinator,
                                         NoiaSurfaceId sid,
                                         NoiaSurfaceId reference_sid,
                                         NoiaPosition pos);

//------------------------------------------------------------------------------

void noia_surface_set_as_cursor(NoiaSurfaceId sid);

int noia_surface_compare(NoiaSurfaceId first, NoiaSurfaceId second);

//------------------------------------------------------------------------------

#endif // NOIA_SURFACE_COORDINATOR_H

