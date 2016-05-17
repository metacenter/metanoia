// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_INPUT_MODE_H
#define NOIA_INPUT_MODE_H

#include "input-binding.h"
#include "global-constants.h"

/// Data type representing input mode.
typedef struct {
    NoiaModeEnum modeid;            ///< ID of the mode.
    void* bindings;                 ///< tree of bindings.
    NoiaBindingExecuteFunc execute; ///< Default execute function.
    bool active;                    ///< Defines if mode is currently active.
} NoiaMode;

/// Construct NoiaMode.
NoiaMode* noia_mode_create(NoiaModeEnum modeid,
                           bool active,
                           NoiaBindingExecuteFunc execute);

/// Destruct NoiaMode.
void noia_mode_destroy(NoiaMode* self);

/// Add new input binding to given mode.
void noia_mode_add_binding(NoiaMode* self, const NoiaBinding* binding);

/// Find binding execute function specified by `code` and `modifiers`
/// in given mode.
/// @return Pointer to execute binding function if found; default function
///         (which may be `NULL`) otherwise.
NoiaBindingExecuteFunc noia_mode_find_binding(NoiaMode* self,
                                              int code,
                                              uint32_t Noiamodifiers);

#endif // NOIA_INPUT_MODE_H

