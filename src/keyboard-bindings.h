// file: keyboard-bindings.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_BINDINGS_H__
#define __NOIA_KEYBOARD_BINDINGS_H__

#include "keyboard-binding.h"
#include "global-constants.h"

/// Add binding `binding` to mode specified by `modeid`.
void noia_keyboard_add_binding(NoiaModeEnum modeid, const NoiaBinding* binding);

/// Make mode specified by `modeid` active or inactive.
void noia_keyboard_mode_make_active(NoiaModeEnum modeid, bool active);

/// Update key binding context and execute key binding specified by `code`.
bool noia_keyboard_catch_key(int code, NoiaKeyState state);

/// Free allocated memory.
void noia_keyboard_free_all();

#endif // __NOIA_KEYBOARD_BINDINGS_H__

