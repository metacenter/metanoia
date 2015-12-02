// file: surface-manager.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_SURFACE_MANAGER_H
#define NOIA_SURFACE_MANAGER_H

#include "surface-data.h"

NoiaSurfaceId noia_surface_create(void);

void noia_surface_destroy(NoiaSurfaceId sid);

NoiaSurfaceData* noia_surface_get(NoiaSurfaceId sid);

void noia_surface_attach_egl(NoiaSurfaceId sid,
                             void* resource);

void noia_surface_commit(NoiaSurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* buffer);

void noia_surface_show(NoiaSurfaceId sid);

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

