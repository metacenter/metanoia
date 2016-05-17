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

void noia_execute(NoiaAction* action)
{
    NoiaObject* copy = (NoiaObject*) noia_action_copy(action);
    noia_object_ref(copy);
    noia_event_signal_emit(SIGNAL_ACTION, copy);
    noia_object_unref(copy);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

static inline void noia_action(NoiaAction* action, NoiaArgmand argmand)
{
    NOIA_ENSURE(action, return);
    NOIA_ENSURE(noia_argmand_is_actionable(argmand), return);

    action->action = argmand;
    if (action->direction != NOIA_ARGMAND_NONE) {
        if (action->magnitude == 0) {
            action->magnitude = 1;
        }
        noia_execute(action);
    }
}

//------------------------------------------------------------------------------

static inline void noia_direction(NoiaAction* action,
                                  NoiaArgmand argmand)
{
    NOIA_ENSURE(action, return);
    NOIA_ENSURE(noia_argmand_is_directed(argmand), return);

    action->direction = argmand;
    if (action->action != NOIA_ARGMAND_NONE) {
        if (action->magnitude == 0) {
            action->magnitude = 1;
        }
        noia_execute(action);
    }
}

//------------------------------------------------------------------------------
// PUBLIC

bool noia_clean_action(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_action_clean(context->action);
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
    NOIA_ENSURE(context, return true);
    NoiaAction* action = context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_FORWARD;
    action->magnitude = 1;
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_cicle_history_back(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    NoiaAction* action = context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_BACK;
    action->magnitude = 1;
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_focus(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_action(context->action, NOIA_ARGMAND_FOCUS);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_swap(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_action(context->action, NOIA_ARGMAND_SWAP);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_move(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_action(context->action, NOIA_ARGMAND_MOVE);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_resize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_action(context->action, NOIA_ARGMAND_RESIZE);
    return true;
}

//------------------------------------------------------------------------------

bool noia_put_number(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);

    int number = noia_get_number_from_key(context->code);
    if (context->action->magnitude == 0) {
        context->action->magnitude = number;
    } else if (number < 0) {
        context->action->magnitude = context->action->magnitude * number;
    } else {
        context->action->magnitude = 10 * context->action->magnitude + number;
    }
    return true;
}

//------------------------------------------------------------------------------

bool noia_focus_workspace(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);

    char str[3];
    int number = noia_get_number_from_key(context->code);
    snprintf(str, sizeof(str), "%d", number);

    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_WORKSPACE;
    action->str = strdup(str);
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_jump_to_workspace(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);

    char str[3];
    int number = noia_get_number_from_key(context->code);
    snprintf(str, sizeof(str), "%d", number);

    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_JUMP;
    action->direction = NOIA_ARGMAND_WORKSPACE;
    action->str = strdup(str);
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_anchorize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_ANCHOR;
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_stackedize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_END;
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_verticalize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_N;
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_horizontalize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_W;
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_select_trunk(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    NoiaAction* action = context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_TRUNK;
    noia_execute(action);
    return true;
}

//------------------------------------------------------------------------------

bool noia_right(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_direction(context->action, NOIA_ARGMAND_E);
    return true;
}

//------------------------------------------------------------------------------

bool noia_left(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_direction(context->action, NOIA_ARGMAND_W);
    return true;
}

//------------------------------------------------------------------------------

bool noia_up(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_direction(context->action, NOIA_ARGMAND_N);
    return true;
}

//------------------------------------------------------------------------------

bool noia_down(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_direction(context->action, NOIA_ARGMAND_S);
    return true;
}

//------------------------------------------------------------------------------

bool noia_back(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_direction(context->action, NOIA_ARGMAND_BACK);
    return true;
}

//------------------------------------------------------------------------------

bool noia_forward(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    noia_direction(context->action, NOIA_ARGMAND_FORWARD);
    return true;
}

//------------------------------------------------------------------------------

bool noia_swap_mode_normal_to_insert(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    LOG_INFO2("Swap mode from normal to insert");

    NoiaList* modes = noia_gears()->modes;
    noia_input_mode_make_active(modes, NOIA_MODE_NORMAL, false);
    noia_input_mode_make_active(modes, NOIA_MODE_INSERT, true);
    return true;
}

//------------------------------------------------------------------------------

bool noia_swap_mode_insert_to_normal(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return true);
    LOG_INFO2("Swap mode from insert to normal");

    NoiaList* modes = noia_gears()->modes;
    noia_input_mode_make_active(modes, NOIA_MODE_INSERT, false);
    noia_input_mode_make_active(modes, NOIA_MODE_NORMAL, true);
    return true;
}

//------------------------------------------------------------------------------

