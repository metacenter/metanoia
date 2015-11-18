// file: keyboard-functions.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_FUNCTIONS_H__
#define __NOIA_KEYBOARD_FUNCTIONS_H__

#include "utils-pool.h"
#include "global-constants.h"

void noia_clean_stack(NoiaPool* stack);

void noia_put_focus(NoiaPool* stack);

void noia_put_move(NoiaPool* stack);

void noia_put_resize(NoiaPool* stack);

void noia_put_number(NoiaPool* stack, int code,
                     uint32_t modifiers, NoiaKeyState state);

void noia_anchorize(NoiaPool* stack);

void noia_right(NoiaPool* stack);

void noia_left(NoiaPool* stack);

void noia_up(NoiaPool* stack);

void noia_down(NoiaPool* stack);

#endif // __NOIA_KEYBOARD_FUNCTIONS_H__

