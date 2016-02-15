// file: surface-data.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-data.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

void noia_surface_data_clean(NoiaSurfaceData* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(NoiaSurfaceData));
    self->buffer.image = EGL_NO_IMAGE_KHR;
    self->pending_buffer.image = EGL_NO_IMAGE_KHR;
}

//------------------------------------------------------------------------------

NoiaSurfaceData* noia_surface_data_new(void)
{
    NoiaSurfaceData* self = malloc(sizeof(NoiaSurfaceData));
    noia_surface_data_clean(self);
    return self;
}

//------------------------------------------------------------------------------

void noia_surface_data_free(NoiaSurfaceData* self)
{
    NOIA_ENSURE(self, return);

    if (self->buffer.data) {
        free(self->buffer.data);
    }

    noia_surface_data_clean(self);
    free(self);
}

//------------------------------------------------------------------------------

