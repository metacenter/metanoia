// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_INPUT_MODE_H
#define NOIA_INPUT_MODE_H

#include "input-binding.h"
#include "global-constants.h"

/// Data type representing input mode.
typedef struct {
    NoiaModeEnum modeid;
    void* bindings;
    bool active;
} NoiaMode;

/// Construct NoiaMode.
NoiaMode* noia_mode_create(NoiaModeEnum modeid);

/// Destruct NoiaMode.
void noia_mode_destroy(NoiaMode* self);

/// Add new input binding to given mode.
void noia_mode_add_binding(NoiaMode* self, const NoiaBinding* binding);

/// Find binding specified by `code` and `modifiers` in given mode.
/// @return Pointer to binding if found; `NULL` otherwise.
NoiaBinding* noia_mode_find_binding(NoiaMode* self,
                                    int code,
                                    uint32_t Noiamodifiers);

#endif // NOIA_INPUT_MODE_H

