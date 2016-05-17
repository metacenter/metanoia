// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_INPUT_FUNCTIONS_H
#define NOIA_INPUT_FUNCTIONS_H

#include "input-context.h"

bool noia_clean_action(NoiaInputContext* context);

bool noia_swallow();

bool noia_quit();

bool noia_refresh_displays();

bool noia_cicle_history_forward(NoiaInputContext* context);

bool noia_cicle_history_back(NoiaInputContext* context);

bool noia_put_focus(NoiaInputContext* context);

bool noia_put_swap(NoiaInputContext* context);

bool noia_put_move(NoiaInputContext* context);

bool noia_put_resize(NoiaInputContext* context);

bool noia_put_number(NoiaInputContext* context);

bool noia_focus_workspace(NoiaInputContext* context);

bool noia_jump_to_workspace(NoiaInputContext* context);

bool noia_anchorize(NoiaInputContext* context);

bool noia_stackedize(NoiaInputContext* context);

bool noia_verticalize(NoiaInputContext* context);

bool noia_horizontalize(NoiaInputContext* context);

bool noia_select_trunk(NoiaInputContext* context);

bool noia_right(NoiaInputContext* context);

bool noia_left(NoiaInputContext* context);

bool noia_up(NoiaInputContext* context);

bool noia_down(NoiaInputContext* context);

bool noia_back(NoiaInputContext* context);

bool noia_forward(NoiaInputContext* context);

bool noia_swap_mode_normal_to_insert(NoiaInputContext* context);

bool noia_swap_mode_insert_to_normal(NoiaInputContext* context);

#endif // NOIA_INPUT_FUNCTIONS_H

