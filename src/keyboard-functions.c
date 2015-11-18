// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-functions.h"
#include "keyboard-argmand.h"
#include "utils-log.h"
#include "exhibitor.h"
#include "global-macros.h"

#include <linux/input.h>

//------------------------------------------------------------------------------

static inline void noia_action(NoiaPool* stack,
                               NoiaArgmandType action)
{
    NoiaArgmand* top = (NoiaArgmand*) noia_pool_top(stack);

    if (top) {
        if (noia_argmand_type_is_directed(top->type)) {
            noia_exhibitor_command_position(action, top->type, top->value);
            noia_pool_drop(stack, 1);
        } else if (top->type == NOIA_ARGMAND_NUMBER) {
            top->type = action;
        } else if (top->type == action) {
            top->value += 1;
        }
    } else {
        NoiaArgmand* argmand = noia_pool_add(stack);
        noia_argmand_init(argmand, action, 1);
    }
}

//------------------------------------------------------------------------------

static inline void noia_direction(NoiaPool* stack,
                                  NoiaArgmandType direction)
{
    NoiaArgmand* top = (NoiaArgmand*) noia_pool_top(stack);

    if (top) {
        if (noia_argmand_type_is_actionable(top->type)) {
            noia_exhibitor_command_position(top->type, direction, top->value);
            noia_pool_drop(stack, 1);
        } else if (top->type == NOIA_ARGMAND_NUMBER) {
            top->type = direction;
        } else if (top->type == direction) {
            top->value += 1;
        }
    } else {
        NoiaArgmand* argmand = noia_pool_add(stack);
        noia_argmand_init(argmand, direction, 1);
    }
}

//------------------------------------------------------------------------------

void noia_clean_stack(NoiaPool* stack)
{
    noia_pool_release(stack);
}

//------------------------------------------------------------------------------

void noia_put_focus(NoiaPool* stack)
{
    noia_action(stack, NOIA_ARGMAND_FOCUS);
}

//------------------------------------------------------------------------------

void noia_put_move(NoiaPool* stack)
{
    noia_action(stack, NOIA_ARGMAND_MOVE);
}

//------------------------------------------------------------------------------

void noia_put_resize(NoiaPool* stack)
{
    noia_action(stack, NOIA_ARGMAND_RESIZE);
}

//------------------------------------------------------------------------------

void noia_put_number(NoiaPool* stack, int code,
                     uint32_t modifiers NOIA_UNUSED,
                     NoiaKeyState state NOIA_UNUSED)
{
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

    NoiaArgmand* top = (NoiaArgmand*) noia_pool_top(stack);
    if (top && top->type == NOIA_ARGMAND_NUMBER) {
        top->value = 10 * top->value + number;
    } else {
        NoiaArgmand* argmand = noia_pool_add(stack);
        noia_argmand_init(argmand, NOIA_ARGMAND_NUMBER, number);
    }
}

//------------------------------------------------------------------------------

void noia_anchorize(NoiaPool* stack)
{
    noia_pool_release(stack);
    noia_exhibitor_command_anchorize();
}

//------------------------------------------------------------------------------

void noia_right(NoiaPool* stack)
{
    noia_direction(stack, NOIA_ARGMAND_E);
}

//------------------------------------------------------------------------------

void noia_left(NoiaPool* stack)
{
    noia_direction(stack, NOIA_ARGMAND_W);
}

//------------------------------------------------------------------------------

void noia_up(NoiaPool* stack)
{
    noia_direction(stack, NOIA_ARGMAND_N);
}

//------------------------------------------------------------------------------

void noia_down(NoiaPool* stack)
{
    noia_direction(stack, NOIA_ARGMAND_S);
}

//------------------------------------------------------------------------------

