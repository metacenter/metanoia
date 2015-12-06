// file: keyboard-functions.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_KEYBOARD_FUNCTIONS_H
#define NOIA_KEYBOARD_FUNCTIONS_H

#include "keyboard-context.h"

void noia_clean_action(NoiaBindingContext* context);

void noia_cicle_history_forward(NoiaBindingContext* context);

void noia_cicle_history_back(NoiaBindingContext* context);

void noia_put_focus(NoiaBindingContext* context);

void noia_put_swap(NoiaBindingContext* context);

void noia_put_move(NoiaBindingContext* context);

void noia_put_resize(NoiaBindingContext* context);

void noia_put_number(NoiaBindingContext* context);

void noia_anchorize(NoiaBindingContext* context);

void noia_stackedize(NoiaBindingContext* context);

void noia_verticalize(NoiaBindingContext* context);

void noia_horizontalize(NoiaBindingContext* context);

void noia_select_trunk(NoiaBindingContext* context);

void noia_right(NoiaBindingContext* context);

void noia_left(NoiaBindingContext* context);

void noia_up(NoiaBindingContext* context);

void noia_down(NoiaBindingContext* context);

void noia_back(NoiaBindingContext* context);

void noia_forward(NoiaBindingContext* context);

void noia_swap_mode_normal_to_insert(NoiaBindingContext* context);

void noia_swap_mode_insert_to_normal(NoiaBindingContext* context);

#endif // NOIA_KEYBOARD_FUNCTIONS_H

