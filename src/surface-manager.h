// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_SURFACE_MANAGER_H
#define NOIA_SURFACE_MANAGER_H

#include "surface-data.h"

NoiaSurfaceId noia_surface_create(void);

void noia_surface_destroy(NoiaSurfaceId sid);

NoiaSurfaceData* noia_surface_get(NoiaSurfaceId sid);

void noia_surface_attach_egl(NoiaSurfaceId sid,
                             void* resource);

void noia_surface_attach(NoiaSurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* buffer,
                         void* resource);

void noia_surface_commit(NoiaSurfaceId sid);

void noia_surface_show(NoiaSurfaceId sid, NoiaSurfaceShowReason reason);

void noia_surface_set_offset(NoiaSurfaceId sid, NoiaPosition offset);

void noia_surface_set_desired_size(NoiaSurfaceId sid,
                                   NoiaSize size);

void noia_surface_set_requested_size(NoiaSurfaceId sid,
                                     NoiaSize size);

void noia_surface_set_as_cursor(NoiaSurfaceId sid);

int noia_surface_compare(NoiaSurfaceId first, NoiaSurfaceId second);

void noia_surface_lock(void);

void noia_surface_unlock(void);

void noia_surface_clear_all(void);

#endif // NOIA_SURFACE_MANAGER_H

