// file: mock-surface-manager.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_MOCK_SURFACE_MANAGER_H
#define NOIA_MOCK_SURFACE_MANAGER_H

#include "surface-manager.h"

void noia_mock_surface_manager_initialize(void);
void noia_mock_surface_manager_finalize(void);

NoiaSize noia_mock_surface_get_desired_size(NoiaSurfaceId sid);

#endif // NOIA_MOCK_SURFACE_MANAGER_H

