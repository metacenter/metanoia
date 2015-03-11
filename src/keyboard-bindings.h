// file: keyboard-bindings.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_BINDINGS_H__
#define __NOIA_KEYBOARD_BINDINGS_H__

#include "keyboard-binding.h"
#include "global-constants.h"

void noia_keyboard_add_binding(NoiaModeEnum modeid, const NoiaBinding* binding);

bool noia_keyboard_catch_key(int code, NoiaKeyState state);

void noia_keyboard_free_all();

#endif // __NOIA_KEYBOARD_BINDINGS_H__

