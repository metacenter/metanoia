// file: keyboard-bindings.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_BINDINGS_H__
#define __AURA_KEYBOARD_BINDINGS_H__

#include "keyboard-binding.h"
#include "global-constants.h"

void aura_keyboard_add_binding(AuraModeEnum modeid, const AuraBinding* binding);

bool aura_keyboard_catch_key(int code, AuraKeyState state);

void aura_keyboard_free_all();

#endif // __AURA_KEYBOARD_BINDINGS_H__

