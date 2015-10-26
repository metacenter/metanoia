// file: keyboard-functions.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_FUNCTIONS_H__
#define __NOIA_KEYBOARD_FUNCTIONS_H__

#include "utils-list.h"

void noia_clean_stack(NoiaList* stack);

void noia_put_focus(NoiaList* stack);

void noia_put_move(NoiaList* stack);

void noia_put_resize(NoiaList* stack);

void noia_put_number(NoiaList* stack, int code,
                     uint32_t modifiers, NoiaKeyState state);

void noia_anchorize(NoiaList* stack);

void noia_right(NoiaList* stack);

void noia_left(NoiaList* stack);

void noia_up(NoiaList* stack);

void noia_down(NoiaList* stack);

#endif // __NOIA_KEYBOARD_FUNCTIONS_H__

