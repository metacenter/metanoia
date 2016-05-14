// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "mock-surface-coordinator.h"
#include "surface-coordinator.h"

#include "global-macros.h"

#include <malloc.h>
#include <assert.h>

typedef struct {
    NoiaItem item;
    NoiaSize size;
} NoiaMockSurfaceData;

struct NoiaCoordinatorStruct {
    NoiaStore* surfaces;
};

//------------------------------------------------------------------------------

NoiaMockSurfaceData* noia_mock_surface_manager_data_create(NoiaSize size)
{
    NoiaMockSurfaceData* data = malloc(sizeof(NoiaMockSurfaceData));
    assert(data);
    data->size = size;
    return data;
}

//------------------------------------------------------------------------------

NoiaCoordinator* noia_coordinator_mock_new(void)
{
    NoiaCoordinator* mock = malloc(sizeof(*mock));
    mock->surfaces = noia_store_new_for_id();
    return mock;
}

//------------------------------------------------------------------------------

void noia_coordinator_mock_free(NoiaCoordinator* mock)
{
    noia_store_free_with_items(mock->surfaces, free);
    free(mock);
}

//------------------------------------------------------------------------------

void noia_surface_set_desired_size(NoiaCoordinator* mock,
                                   NoiaSurfaceId sid,
                                   NoiaSize size)
{
    NoiaMockSurfaceData* data = noia_store_find(mock->surfaces, sid);
    if (not data) {
        data = noia_mock_surface_manager_data_create(size);
    }
    data->size = size;
    noia_store_add_with_id(mock->surfaces, sid, data);
}

//------------------------------------------------------------------------------

NoiaSize noia_surface_get_desired_size(NoiaCoordinator* mock,
                                       NoiaSurfaceId sid)
{
    NoiaSize size = {0, 0};
    NoiaMockSurfaceData* data = noia_store_find(mock->surfaces, sid);
    if (data) {
        size = data->size;
    }
    return size;
}

//------------------------------------------------------------------------------

