// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_INPUT_CONTEXT_H
#define NOIA_INPUT_CONTEXT_H

#include "global-objects.h"
#include "global-constants.h"

/// Context passed to input binding handlers.
typedef struct {
    NoiaCommand* command;
    int code;
    uint32_t modifiers;
    NoiaKeyState state;
    uint32_t properties;
} NoiaInputContext;

/// Constructor of `NoiaInputContext`.
NoiaInputContext* noia_input_context_create(uint32_t properties);

/// Destructor of `NoiaInputContext`.
void noia_input_context_destroy(NoiaInputContext* self);

#endif // NOIA_INPUT_CONTEXT_H

