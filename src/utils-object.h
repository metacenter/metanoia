// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_OBJECT_H
#define NOIA_OBJECT_H

#include "global-constants.h"

/// Reference count implementation.
typedef struct {
    int refcount;
    NoiaFreeFunc free;
} NoiaObject;

void noia_object_initialize(NoiaObject* self, NoiaFreeFunc free);

void noia_object_ref(NoiaObject* self);

void noia_object_unref(NoiaObject* self);

#endif // NOIA_OBJECT_H

