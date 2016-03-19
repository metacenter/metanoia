// file: input-bindings.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_INPUT_BINDINGS_H
#define NOIA_INPUT_BINDINGS_H

#include "input-binding.h"
#include "global-constants.h"

/// Add binding `binding` to mode specified by `modeid`.
void noia_input_add_binding(NoiaList* modes,
                            NoiaModeEnum modeid,
                            const NoiaBinding* binding);

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

