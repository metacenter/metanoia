// file: input-functions.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_INPUT_FUNCTIONS_H
#define NOIA_INPUT_FUNCTIONS_H

#include "input-context.h"

void noia_clean_action(NoiaInputContext* context);

void noia_cicle_history_forward(NoiaInputContext* context);

void noia_cicle_history_back(NoiaInputContext* context);

void noia_put_focus(NoiaInputContext* context);

void noia_put_swap(NoiaInputContext* context);

void noia_put_move(NoiaInputContext* context);

void noia_put_resize(NoiaInputContext* context);

void noia_put_number(NoiaInputContext* context);

void noia_focus_workspace(NoiaInputContext* context);

void noia_anchorize(NoiaInputContext* context);

void noia_stackedize(NoiaInputContext* context);

void noia_verticalize(NoiaInputContext* context);

void noia_horizontalize(NoiaInputContext* context);

void noia_select_trunk(NoiaInputContext* context);

void noia_right(NoiaInputContext* context);

void noia_left(NoiaInputContext* context);

void noia_up(NoiaInputContext* context);

void noia_down(NoiaInputContext* context);

void noia_back(NoiaInputContext* context);

void noia_forward(NoiaInputContext* context);

void noia_swap_mode_normal_to_insert(NoiaInputContext* context);

void noia_swap_mode_insert_to_normal(NoiaInputContext* context);

#endif // NOIA_INPUT_FUNCTIONS_H

