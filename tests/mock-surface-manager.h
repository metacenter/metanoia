// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_MOCK_SURFACE_MANAGER_H
#define NOIA_MOCK_SURFACE_MANAGER_H

#include "surface-manager.h"

void noia_mock_surface_manager_initialize(void);
void noia_mock_surface_manager_finalize(void);

NoiaSize noia_mock_surface_get_desired_size(NoiaSurfaceId sid);

#endif // NOIA_MOCK_SURFACE_MANAGER_H

