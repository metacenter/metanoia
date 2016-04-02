// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "input-context.h"

//------------------------------------------------------------------------------

NoiaInputContext* noia_input_context_create(void)
{
    NoiaInputContext* self = malloc(sizeof(NoiaInputContext));
    NOIA_ENSURE(self, abort());

    self->action = noia_action_create();
    self->code = 0;
    self->modifiers = 0x0;
    self->state = NOIA_KEY_RELEASED;
    return self;
}

//------------------------------------------------------------------------------

void noia_input_context_destroy(NoiaInputContext* self)
{
    NOIA_ENSURE(self, return);
    noia_action_destroy(self->action);
    free(self);
}

//------------------------------------------------------------------------------

