// file: keyboard-functions.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_FUNCTIONS_H__
#define __NOIA_KEYBOARD_FUNCTIONS_H__

#include "global-constants.h"

void noia_cicle_history_forward(NoiaAction* action);

void noia_cicle_history_back(NoiaAction* action);

void noia_put_focus(NoiaAction* action);

void noia_put_move(NoiaAction* action);

void noia_put_resize(NoiaAction* action);

void noia_put_number(NoiaAction* action, int code,
                     uint32_t modifiers, NoiaKeyState state);

void noia_anchorize(NoiaAction* action);

void noia_select_above(NoiaAction* action);

void noia_right(NoiaAction* action);

void noia_left(NoiaAction* action);

void noia_up(NoiaAction* action);

void noia_down(NoiaAction* action);

#endif // __NOIA_KEYBOARD_FUNCTIONS_H__

