// file: surface-data.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-data.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

AuraSurfaceData* aura_surface_data_new()
{
    AuraSurfaceData* self = malloc(sizeof(AuraSurfaceData));
    if (!self) {
        LOG_ERROR("Could not create new surface!");
        return 0;
    }

    memset(self, 0, sizeof(AuraSurfaceData));

    return self;
}

//------------------------------------------------------------------------------

void aura_surface_data_free(AuraSurfaceData* self)
{
    if (!self) {
        return;
    }

    memset(self, 0, sizeof(AuraSurfaceData));
    free(self);
}

//------------------------------------------------------------------------------

