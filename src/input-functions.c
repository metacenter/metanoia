// file: input-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "input-functions.h"
#include "input-bindings.h"
#include "input-mode.h"
#include "config.h"
#include "utils-log.h"
#include "exhibitor-module.h"
#include "global-macros.h"
#include "event-signals.h"

#include <string.h>
#include <linux/input.h>

//------------------------------------------------------------------------------

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

void noia_clean_action(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action_clean(context->action);
}

//------------------------------------------------------------------------------

void noia_cicle_history_forward(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_FORWARD;
    action->magnitude = 1;
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_cicle_history_back(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_BACK;
    action->magnitude = 1;
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_put_focus(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(context->action, NOIA_ARGMAND_FOCUS);
}

//------------------------------------------------------------------------------

void noia_put_swap(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(context->action, NOIA_ARGMAND_SWAP);
}

//------------------------------------------------------------------------------

void noia_put_move(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(context->action, NOIA_ARGMAND_MOVE);
}

//------------------------------------------------------------------------------

void noia_put_resize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(context->action, NOIA_ARGMAND_RESIZE);
}

//------------------------------------------------------------------------------

void noia_put_number(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);

    int number = noia_get_number_from_key(context->code);
    if (context->action->magnitude == 0) {
        context->action->magnitude = number;
    } else if (number < 0) {
        context->action->magnitude = context->action->magnitude * number;
    } else {
        context->action->magnitude = 10 * context->action->magnitude + number;
    }
}

//------------------------------------------------------------------------------

void noia_focus_workspace(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);

    char str[3];
    int number = noia_get_number_from_key(context->code);
    snprintf(str, sizeof(str), "%d", number);

    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_WORKSPACE;
    action->str = strdup(str);
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_anchorize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_ANCHOR;
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_stackedize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_END;
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_verticalize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_N;
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_horizontalize(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_W;
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_select_trunk(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_TRUNK;
    noia_execute(action);
}

//------------------------------------------------------------------------------

void noia_right(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(context->action, NOIA_ARGMAND_E);
}

//------------------------------------------------------------------------------

void noia_left(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(context->action, NOIA_ARGMAND_W);
}

//------------------------------------------------------------------------------

void noia_up(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(context->action, NOIA_ARGMAND_N);
}

//------------------------------------------------------------------------------

void noia_down(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(context->action, NOIA_ARGMAND_S);
}

//------------------------------------------------------------------------------

void noia_back(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(context->action, NOIA_ARGMAND_BACK);
}

//------------------------------------------------------------------------------

void noia_forward(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(context->action, NOIA_ARGMAND_FORWARD);
}

//------------------------------------------------------------------------------

void noia_swap_mode_normal_to_insert(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    LOG_INFO2("Swap mode from normal to insert");

    NoiaList* modes = noia_gears()->modes;
    noia_input_mode_make_active(modes, NOIA_MODE_NORMAL, false);
    noia_input_mode_make_active(modes, NOIA_MODE_INSERT, true);
}

//------------------------------------------------------------------------------

void noia_swap_mode_insert_to_normal(NoiaInputContext* context)
{
    NOIA_ENSURE(context, return);
    LOG_INFO2("Swap mode from insert to normal");

    NoiaList* modes = noia_gears()->modes;
    noia_input_mode_make_active(modes, NOIA_MODE_INSERT, false);
    noia_input_mode_make_active(modes, NOIA_MODE_NORMAL, true);
}

//------------------------------------------------------------------------------

