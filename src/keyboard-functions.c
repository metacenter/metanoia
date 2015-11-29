// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-functions.h"
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
        noia_exhibitor_execute(action);
        noia_action_clean(action);
    }
}

//------------------------------------------------------------------------------

void noia_cicle_history_forward(NoiaAction* action)
{
    NOIA_ENSURE(action, return);
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_FORWARD;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_cicle_history_back(NoiaAction* action)
{
    NOIA_ENSURE(action, return);
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_FORWARD;
    action->magnitude = -1 * action->magnitude;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_put_focus(NoiaAction* action)
{
    noia_action(action, NOIA_ARGMAND_FOCUS);
}

//------------------------------------------------------------------------------

void noia_put_move(NoiaAction* action)
{
    noia_action(action, NOIA_ARGMAND_MOVE);
}

//------------------------------------------------------------------------------

void noia_put_resize(NoiaAction* action)
{
    noia_action(action, NOIA_ARGMAND_RESIZE);
}

//------------------------------------------------------------------------------

void noia_put_number(NoiaAction* action, int code,
                     uint32_t modifiers NOIA_UNUSED,
                     NoiaKeyState state NOIA_UNUSED)
{
    NOIA_ENSURE(action, return);

    /// @todo Move elsewhere.
    int number = 0;
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

    action->magnitude = 10 * action->magnitude + number;
}

//------------------------------------------------------------------------------

void noia_anchorize(NoiaAction* action)
{
    NOIA_ENSURE(action, return);
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_ANCHOR;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_stackedize(NoiaAction* action)
{
    NOIA_ENSURE(action, return);
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_END;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_verticalize(NoiaAction* action)
{
    NOIA_ENSURE(action, return);
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_N;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_horizontalize(NoiaAction* action)
{
    NOIA_ENSURE(action, return);
    noia_action_clean(action);
    action->action = NOIA_ARGMAND_CONF;
    action->direction = NOIA_ARGMAND_W;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_select_trunk(NoiaAction* action)
{
    NOIA_ENSURE(action, return);
    action->action = NOIA_ARGMAND_FOCUS;
    action->direction = NOIA_ARGMAND_TRUNK;
    noia_exhibitor_execute(action);
    noia_action_clean(action);
}

//------------------------------------------------------------------------------

void noia_right(NoiaAction* action)
{
    noia_direction(action, NOIA_ARGMAND_E);
}

//------------------------------------------------------------------------------

void noia_left(NoiaAction* action)
{
    noia_direction(action, NOIA_ARGMAND_W);
}

//------------------------------------------------------------------------------

void noia_up(NoiaAction* action)
{
    noia_direction(action, NOIA_ARGMAND_N);
}

//------------------------------------------------------------------------------

void noia_down(NoiaAction* action)
{
    noia_direction(action, NOIA_ARGMAND_S);
}

//------------------------------------------------------------------------------

