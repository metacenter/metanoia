// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-output.h"

#include "controller-defs.h"

#include <malloc.h>

static int scLastID = 0;

//------------------------------------------------------------------------------

NoiaCtlOutput* noia_controller_output_new(void)
{
    NoiaCtlOutput* self = malloc(sizeof(*self));
    assert(self);

    memset(self, 0, sizeof(*self));
    self->id = ++scLastID;

    return self;
}

//------------------------------------------------------------------------------

