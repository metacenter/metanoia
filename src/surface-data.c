// file: surface-data.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-data.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

/// Allocate Surface Data
NoiaSurfaceData* noia_surface_data_new()
{
    NoiaSurfaceData* self = malloc(sizeof(NoiaSurfaceData));
    if (!self) {
        LOG_ERROR("Could not create new surface!");
        return 0;
    }

    memset(self, 0, sizeof(NoiaSurfaceData));

    return self;
}

//------------------------------------------------------------------------------

/// Deallocate Surface Data.
void noia_surface_data_free(NoiaSurfaceData* self)
{
    if (!self) {
        return;
    }

    memset(self, 0, sizeof(NoiaSurfaceData));
    free(self);
}

//------------------------------------------------------------------------------

