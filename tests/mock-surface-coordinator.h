// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_MOCK_SURFACE_COORDINATOR_H
#define NOIA_MOCK_SURFACE_COORDINATOR_H

#include "global-types.h"

struct NoiaCoordinatorStruct;

struct NoiaCoordinatorStruct* noia_coordinator_mock_new(void);
void noia_coordinator_mock_free(struct NoiaCoordinatorStruct* mock);

NoiaSize noia_surface_get_desired_size(struct NoiaCoordinatorStruct* mock,
                                       NoiaSurfaceId sid);

#endif // NOIA_MOCK_SURFACE_COORDINATOR_H

