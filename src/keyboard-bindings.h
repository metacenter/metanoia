// file: keyboard-bindings.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_BINDINGS_H__
#define __AURA_KEYBOARD_BINDINGS_H__

#include "global-constants.h"

void aura_keyboard_add_binding(const Binding* binding);

void aura_keyboard_remove_binding(uint32_t code,
                                  uint32_t modifiers);

int aura_keyboard_catch_key(int code,
                            KeyState state);

#endif // __AURA_KEYBOARD_BINDINGS_H__

