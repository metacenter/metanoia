// file: keyboard-functions.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_FUNCTIONS_H__
#define __AURA_KEYBOARD_FUNCTIONS_H__

#include "utils-chain.h"

void aura_clean_stack(Chain* stack);

void aura_put_focus(Chain* stack);

void aura_put_move(Chain* stack);

void aura_put_resize(Chain* stack);

void aura_right(Chain* stack);

void aura_left(Chain* stack);

void aura_up(Chain* stack);

void aura_down(Chain* stack);

#endif // __AURA_KEYBOARD_FUNCTIONS_H__

