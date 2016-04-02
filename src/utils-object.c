// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-object.h"

//------------------------------------------------------------------------------

/// Initialize object by setting free function and zeroing reference count.
void noia_object_initialize(NoiaObject* self, NoiaFreeFunc free)
{
    if (!self) {
        return;
    }

    self->refcount = 0;
    self->free = free;
}

//------------------------------------------------------------------------------

/// Increase reference count by one.
void noia_object_ref(NoiaObject* self)
{
    if (!self) {
        return;
    }

    self->refcount += 1;
}

//------------------------------------------------------------------------------

/// Decrease reference count by one and free object if nothing references it.
void noia_object_unref(NoiaObject* self)
{
    if (!self) {
        return;
    }

    self->refcount -= 1;

    if (self->refcount < 1) {
        self->free(self);
    }
}

//------------------------------------------------------------------------------

