// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-store.h"

#include <malloc.h>
#include <assert.h>

typedef struct {
    NoiaItem item;
    NoiaSize size;
} NoiaMockSurfaceData;

static NoiaStore* sSurfaceStore = NULL;

//------------------------------------------------------------------------------

NoiaMockSurfaceData* noia_mock_surface_manager_data_create(NoiaSize size)
{
    NoiaMockSurfaceData* data = malloc(sizeof(NoiaMockSurfaceData));
    assert(data);
    data->size = size;
    return data;
}

//------------------------------------------------------------------------------

void noia_mock_surface_manager_initialize(void)
{
    sSurfaceStore = noia_store_new_for_id();
}

//------------------------------------------------------------------------------

void noia_mock_surface_manager_finalize(void)
{
    noia_store_free_with_items(sSurfaceStore, free);
}

//------------------------------------------------------------------------------

void noia_surface_set_desired_size(NoiaSurfaceId sid, NoiaSize size)
{
    NoiaMockSurfaceData* data = noia_store_find(sSurfaceStore, sid);
    if (!data) {
        data = noia_mock_surface_manager_data_create(size);
    }
    data->size = size;
    noia_store_add_with_id(sSurfaceStore, sid, data);
}

//------------------------------------------------------------------------------

NoiaSize noia_mock_surface_get_desired_size(NoiaSurfaceId sid)
{
    NoiaSize size = {0, 0};
    NoiaMockSurfaceData* data = noia_store_find(sSurfaceStore, sid);
    if (data) {
        size = data->size;
    }
    return size;
}

//------------------------------------------------------------------------------

