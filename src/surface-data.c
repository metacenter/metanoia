// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "surface-data.h"
#include "global-macros.h"

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

