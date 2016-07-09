// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "input-context.h"
#include "global-macros.h"

#include <memory.h>

//------------------------------------------------------------------------------

NoiaInputContext* noia_input_context_create(uint32_t properties)
{
    NoiaInputContext* self = malloc(sizeof(NoiaInputContext));
    NOIA_ENSURE(self, abort());

    memset(self, 0, sizeof(*self));
    self->command = noia_command_create();
    self->properties = properties;
    return self;
}

//------------------------------------------------------------------------------

void noia_input_context_destroy(NoiaInputContext* self)
{
    NOIA_ENSURE(self, return);
    noia_command_destroy(self->command);
    free(self);
}

//------------------------------------------------------------------------------

