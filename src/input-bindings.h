// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_INPUT_BINDINGS_H
#define NOIA_INPUT_BINDINGS_H

#include "input-binding.h"
#include "global-constants.h"
#include "utils-list.h"

/// Make mode specified by `modeid` active or inactive.
void noia_input_mode_make_active(NoiaList* modes,
                                 NoiaModeEnum modeid,
                                 bool active);

/// Update key binding context and execute key binding specified by `code`.
/// @return `true` if key was cached, `false` otherwise.
bool noia_input_catch_key(NoiaList* modes,
                          NoiaInputContext* context,
                          int code,
                          NoiaKeyState state);

#endif // NOIA_INPUT_BINDINGS_H

