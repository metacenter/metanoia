// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_INPUT_BINDING_H
#define NOIA_INPUT_BINDING_H

#include "input-context.h"

typedef void (*NoiaBindingExecuteFunc) (NoiaInputContext*);

/// Data type representing key binding.
typedef struct {
    int code;                       ///< Key code.
    uint32_t modifiers;             ///< Active modifiers.
    NoiaBindingExecuteFunc execute; ///< Binding execution callback.
} NoiaBinding;

/// Free binding.
void noia_binding_free(NoiaBinding* self);

/// Copy binding.
NoiaBinding* noia_binding_copy(const NoiaBinding* self);

/// Compare two bindings.
/// @return `0` if code and modifiers are the same.
int noia_binding_compare(const NoiaBinding* binding1,
                         const NoiaBinding* binding2);

#endif // NOIA_INPUT_BINDING_H

