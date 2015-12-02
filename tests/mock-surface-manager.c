// file: mock-surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

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
    noia_store_add_with_id(sSurfaceStore, sid, data);
}

//------------------------------------------------------------------------------

