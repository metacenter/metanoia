// file: keyboard-functions.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_FUNCTIONS_H__
#define __AURA_KEYBOARD_FUNCTIONS_H__

#include "utils-list.h"

void aura_clean_stack(AuraList* stack);

void aura_put_focus(AuraList* stack);

void aura_put_move(AuraList* stack);

void aura_put_resize(AuraList* stack);

void aura_put_number(AuraList* stack, int code,
                     uint32_t modifiers, AuraKeyState state);

void aura_right(AuraList* stack);

void aura_left(AuraList* stack);

void aura_up(AuraList* stack);

void aura_down(AuraList* stack);

#endif // __AURA_KEYBOARD_FUNCTIONS_H__

