// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-output.h"

#include <memory.h>
#include <malloc.h>

static int sLastOutputId = 0;

//------------------------------------------------------------------------------

NoiaCtlOutput* noia_controller_output_new(void)
{
    NoiaCtlOutput* self = malloc(sizeof(*self));
    memset(self, 0, sizeof(*self));
    self->id = sLastOutputId++;
    return self;
}

//------------------------------------------------------------------------------

void noia_controller_output_free(NoiaCtlOutput* self)
{
    if (self) {
        if (self->output) {
            wl_output_destroy(self->output);
        }
        if (self->buffer) {
            wl_buffer_destroy(self->buffer);
        }
        free(self);
    }
}

//------------------------------------------------------------------------------

