// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "input-functions.h"
#include "input-bindings.h"
#include "input-mode.h"
#include "config.h"
#include "utils-log.h"
#include "exhibitor-module.h"
#include "global-macros.h"
#include "event-signals.h"

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <linux/input.h>

//------------------------------------------------------------------------------
// PRIVATE

int noia_get_number_from_key(int code)
{
    int number = -2;
    switch (code) {
        case KEY_MINUS: number = -1; break;
        case KEY_0: case KEY_NUMERIC_0: number = 0; break;
        case KEY_1: case KEY_NUMERIC_1: number = 1; break;
        case KEY_2: case KEY_NUMERIC_2: number = 2; break;
        case KEY_3: case KEY_NUMERIC_3: number = 3; break;
        case KEY_4: case KEY_NUMERIC_4: number = 4; break;
        case KEY_5: case KEY_NUMERIC_5: number = 5; break;
        case KEY_6: case KEY_NUMERIC_6: number = 6; break;
        case KEY_7: case KEY_NUMERIC_7: number = 7; break;
        case KEY_8: case KEY_NUMERIC_8: number = 8; break;
        case KEY_9: case KEY_NUMERIC_9: number = 9; break;
    }
    return number;
}

//------------------------------------------------------------------------------

void noia_execute(NoiaCommand* command)
{
    NoiaObject* copy = (NoiaObject*) noia_command_copy(command);
    noia_object_ref(copy);
    noia_event_signal_emit(SIGNAL_ACTION, copy);
    noia_object_unref(copy);
    noia_command_clean(command);
}

//------------------------------------------------------------------------------

static inline void noia_put_action(NoiaCommand* command, NoiaAction action)
{
    command->action = action;
    if (command->direction != NOIA_DIRECTION_NONE) {
        if (command->magnitude == 0) {
            command->magnitude = 1;
        }
        noia_execute(command);
    }
}

//------------------------------------------------------------------------------

static inline void noia_put_direction(NoiaCommand* command,
                                      NoiaDirection direction)
{
    command->direction = direction;
    if (command->action != NOIA_ACTION_NONE) {
        if (command->magnitude == 0) {
            command->magnitude = 1;
        }
        noia_execute(command);
    }
}

//------------------------------------------------------------------------------
// PUBLIC

bool noia_clean_command(NoiaInputContext* context)
{
    noia_command_clean(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_swallow()
{
    return true;
}

//------------------------------------------------------------------------------

bool noia_quit()
{
    LOG_INFO1("QUIT!");
    kill(getpid(), SIGTERM);
    return true;
}

//------------------------------------------------------------------------------

bool noia_refresh_displays()
{
    noia_event_signal_emit(SIGNAL_DISPLAYS_CHANGED, NULL);
    return true;
}

//------------------------------------------------------------------------------

bool noia_cicle_history_forward(NoiaInputContext* context)
{
    context->command->action = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_FORWARD;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_cicle_history_back(NoiaInputContext* context)
{
    context->command->action = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_BACK;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_focus(NoiaInputContext* context)
{
    noia_put_action(context->command, NOIA_ACTION_FOCUS);
    return true;
}

//------------------------------------------------------------------------------

bool noia_focus_right(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_E;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_focus_down(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_S;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_focus_left(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_W;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_focus_up(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_N;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_swap(NoiaInputContext* context)
{
    noia_put_action(context->command, NOIA_ACTION_SWAP);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_jump(NoiaInputContext* context)
{
    noia_put_action(context->command, NOIA_ACTION_JUMP);
    return true;
}

//------------------------------------------------------------------------------

bool noia_jump_right(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_JUMP;
    context->command->direction = NOIA_DIRECTION_E;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_jump_down(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_JUMP;
    context->command->direction = NOIA_DIRECTION_S;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_jump_left(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_JUMP;
    context->command->direction = NOIA_DIRECTION_W;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_jump_up(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_JUMP;
    context->command->direction = NOIA_DIRECTION_N;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_dive(NoiaInputContext* context)
{
    noia_put_action(context->command, NOIA_ACTION_DIVE);
    return true;
}

//------------------------------------------------------------------------------

bool noia_dive_right(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_DIVE;
    context->command->direction = NOIA_DIRECTION_E;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_dive_down(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_DIVE;
    context->command->direction = NOIA_DIRECTION_S;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_dive_left(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_DIVE;
    context->command->direction = NOIA_DIRECTION_W;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_dive_up(NoiaInputContext* context)
{
    context->command->action    = NOIA_ACTION_DIVE;
    context->command->direction = NOIA_DIRECTION_N;
    context->command->magnitude = 1;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_move(NoiaInputContext* context)
{
    noia_put_action(context->command, NOIA_ACTION_MOVE);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_resize(NoiaInputContext* context)
{
    noia_put_action(context->command, NOIA_ACTION_RESIZE);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_number(NoiaInputContext* context)
{
    int number = noia_get_number_from_key(context->code);
    if (context->command->magnitude == 0) {
        context->command->magnitude = number;
    } else if (number < 0) {
        context->command->magnitude = context->command->magnitude * number;
    } else {
        context->command->magnitude = 10 * context->command->magnitude + number;
    }
    return true;
}

//------------------------------------------------------------------------------

bool noia_focus_workspace(NoiaInputContext* context)
{
    char str[3];
    int number = noia_get_number_from_key(context->code);
    snprintf(str, sizeof(str), "%d", number);

    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_WORKSPACE;
    context->command->str = strdup(str);
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_jump_to_workspace(NoiaInputContext* context)
{
    char str[3];
    int number = noia_get_number_from_key(context->code);
    snprintf(str, sizeof(str), "%d", number);

    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_JUMP;
    context->command->direction = NOIA_DIRECTION_WORKSPACE;
    context->command->str = strdup(str);
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_anchorize(NoiaInputContext* context)
{
    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_ANCHOR;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_ramify(NoiaInputContext* context)
{
    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_JUMP;
    context->command->direction = NOIA_DIRECTION_END;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_enlarge(NoiaInputContext* context)
{
    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_JUMP;
    context->command->direction = NOIA_DIRECTION_BEGIN;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_stackedize(NoiaInputContext* context)
{
    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_CONF;
    context->command->direction = NOIA_DIRECTION_END;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_verticalize(NoiaInputContext* context)
{
    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_CONF;
    context->command->direction = NOIA_DIRECTION_N;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_horizontalize(NoiaInputContext* context)
{
    noia_command_clean(context->command);
    context->command->action = NOIA_ACTION_CONF;
    context->command->direction = NOIA_DIRECTION_W;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_select_trunk(NoiaInputContext* context)
{
    context->command->action = NOIA_ACTION_FOCUS;
    context->command->direction = NOIA_DIRECTION_TRUNK;
    noia_execute(context->command);
    return true;
}

//------------------------------------------------------------------------------

bool noia_right(NoiaInputContext* context)
{
    noia_put_direction(context->command, NOIA_DIRECTION_E);
    return true;
}

//------------------------------------------------------------------------------

bool noia_left(NoiaInputContext* context)
{
    noia_put_direction(context->command, NOIA_DIRECTION_W);
    return true;
}

//------------------------------------------------------------------------------

bool noia_up(NoiaInputContext* context)
{
    noia_put_direction(context->command, NOIA_DIRECTION_N);
    return true;
}

//------------------------------------------------------------------------------

bool noia_down(NoiaInputContext* context)
{
    noia_put_direction(context->command, NOIA_DIRECTION_S);
    return true;
}

//------------------------------------------------------------------------------

bool noia_back(NoiaInputContext* context)
{
    noia_put_direction(context->command, NOIA_DIRECTION_BACK);
    return true;
}

//------------------------------------------------------------------------------

bool noia_forward(NoiaInputContext* context)
{
    noia_put_direction(context->command, NOIA_DIRECTION_FORWARD);
    return true;
}

//------------------------------------------------------------------------------

bool noia_swap_mode_normal_to_insert(NoiaInputContext* context NOIA_UNUSED)
{
    LOG_INFO2("Swap mode from normal to insert");

    NoiaList* modes = noia_gears()->modes;
    noia_input_mode_make_active(modes, NOIA_MODE_NORMAL, false);
    noia_input_mode_make_active(modes, NOIA_MODE_INSERT, true);
    return true;
}

//------------------------------------------------------------------------------

bool noia_swap_mode_insert_to_normal(NoiaInputContext* context NOIA_UNUSED)
{
    NOIA_ENSURE(context, return true);
    LOG_INFO2("Swap mode from insert to normal");

    NoiaList* modes = noia_gears()->modes;
    noia_input_mode_make_active(modes, NOIA_MODE_INSERT, false);
    noia_input_mode_make_active(modes, NOIA_MODE_NORMAL, true);
    return true;
}

//------------------------------------------------------------------------------

