// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "input-binding.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

NoiaBinding* noia_binding_new(void)
{
    NoiaBinding* self = malloc(sizeof(NoiaBinding));
    NOIA_ENSURE(self, abort());

    self->code = 0;
    self->modifiers = 0;
    self->execute = NULL;
    return self;
}

//------------------------------------------------------------------------------

void noia_binding_free(NoiaBinding* self)
{
    NOIA_ENSURE(self, return);
    free(self);
}

//------------------------------------------------------------------------------

NoiaBinding* noia_binding_copy(const NoiaBinding* self)
{
    NoiaBinding* copy = noia_binding_new();
    copy->code      = self->code;
    copy->modifiers = self->modifiers;
    copy->execute   = self->execute;
    return copy;
}

//------------------------------------------------------------------------------

int noia_binding_compare(const NoiaBinding* binding1,
                         const NoiaBinding* binding2)
{
    if (binding1->modifiers < binding2->modifiers) return -1;
    if (binding1->modifiers > binding2->modifiers) return  1;

    if (binding1->code < binding2->code) return -1;
    if (binding1->code > binding2->code) return  1;

    return 0;
}

//------------------------------------------------------------------------------

