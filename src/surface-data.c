// file: surface-data.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-data.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaSurfaceData* noia_surface_data_new(void)
{
    NoiaSurfaceData* self = malloc(sizeof(NoiaSurfaceData));
    NOIA_ENSURE(self, abort());
    memset(self, 0, sizeof(NoiaSurfaceData));
    return self;
}

//------------------------------------------------------------------------------

void noia_surface_data_free(NoiaSurfaceData* self)
{
    NOIA_ENSURE(self, return);

    if (self->buffer.data) {
        free(self->buffer.data);
    }

    memset(self, 0, sizeof(NoiaSurfaceData));
    free(self);
}

//------------------------------------------------------------------------------

