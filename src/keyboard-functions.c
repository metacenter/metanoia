// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-functions.h"
#include "keyboard-mode.h"
#include "utils-log.h"
#include "exhibitor.h"
#include "global-macros.h"

#include <linux/input.h>

//------------------------------------------------------------------------------

static inline void noia_action(NoiaAction* action,
                               NoiaArgmand argmand)
{
    NOIA_ENSURE(action, return);
    NOIA_ENSURE(noia_argmand_is_actionable(argmand), return);

    action->action = argmand;
    if (action->direction != NOIA_ARGMAND_NONE) {
        if (action->magnitude == 0) {
            action->magnitude = 1;
        }
        noia_exhibitor_execute(action);
        noia_action_clean(action);
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
        noia_exhibitor_execute(action);
        noia_action_clean(action);
    }
}

//------------------------------------------------------------------------------

void noia_clean_action(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action_clean(&context->action);
}

//------------------------------------------------------------------------------

void noia_cicle_history_forward(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = &context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_FORWARD;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_cicle_history_back(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = &context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_FORWARD;
    action->magnitude = -1 * action->magnitude;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_put_focus(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(&context->action, NOIA_ARGMAND_FOCUS);
}

//------------------------------------------------------------------------------

void noia_put_swap(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(&context->action, NOIA_ARGMAND_SWAP);
}

//------------------------------------------------------------------------------

void noia_put_move(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(&context->action, NOIA_ARGMAND_MOVE);
}

//------------------------------------------------------------------------------

void noia_put_resize(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_action(&context->action, NOIA_ARGMAND_RESIZE);
}

//------------------------------------------------------------------------------

void noia_put_number(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);

    /// @todo Move elsewhere.
    int number = 0;
    switch (context->code) {
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

    if (context->action.magnitude == 0) {
        context->action.magnitude = number;
    } else if (number < 0) {
        context->action.magnitude = context->action.magnitude * number;
    } else {
        context->action.magnitude = 10 * context->action.magnitude + number;
    }
}

//------------------------------------------------------------------------------

void noia_anchorize(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = &context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_ANCHOR;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_stackedize(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = &context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_END;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_verticalize(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = &context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_N;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_horizontalize(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = &context->action;
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_W;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_select_trunk(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    NoiaAction* action = &context->action;
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_TRUNK;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_right(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(&context->action, NOIA_ARGMAND_E);
}

//------------------------------------------------------------------------------

void noia_left(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(&context->action, NOIA_ARGMAND_W);
}

//------------------------------------------------------------------------------

void noia_up(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(&context->action, NOIA_ARGMAND_N);
}

//------------------------------------------------------------------------------

void noia_down(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(&context->action, NOIA_ARGMAND_S);
}

//------------------------------------------------------------------------------

void noia_back(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(&context->action, NOIA_ARGMAND_BACK);
}

//------------------------------------------------------------------------------

void noia_forward(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    noia_direction(&context->action, NOIA_ARGMAND_FORWARD);
}

//------------------------------------------------------------------------------

void noia_swap_mode_normal_to_insert(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    LOG_INFO2("Swap mode from normal to insert");
    FOR_EACH (context->modes, link) {
        NoiaMode* mode = (NoiaMode*) link->data;
        if (mode->modeid == NOIA_MODE_NORMAL) {
            mode->active = false;
        }
        if (mode->modeid == NOIA_MODE_INSERT) {
            mode->active = true;
        }
    }
}

//------------------------------------------------------------------------------

void noia_swap_mode_insert_to_normal(NoiaBindingContext* context)
{
    NOIA_ENSURE(context, return);
    LOG_INFO2("Swap mode from insert to normal");
    FOR_EACH (context->modes, link) {
        NoiaMode* mode = (NoiaMode*) link->data;
        if (mode->modeid == NOIA_MODE_INSERT) {
            mode->active = false;
        }
        if (mode->modeid == NOIA_MODE_NORMAL) {
            mode->active = true;
        }
    }
}

//------------------------------------------------------------------------------

