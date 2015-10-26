// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-functions.h"
#include "keyboard-argmand.h"
#include "utils-log.h"
#include "exhibitor.h"

#include <linux/input.h>

//------------------------------------------------------------------------------

void noia_clean_stack(NoiaList* stack)
{
    noia_list_clean(stack);
}

//------------------------------------------------------------------------------

void noia_put_focus(NoiaList* stack)
{
    noia_list_append(stack, noia_argmand_new(NOIA_ARGMAND_FOCUS, 0));
}

//------------------------------------------------------------------------------

void noia_put_move(NoiaList* stack)
{
    noia_list_append(stack, noia_argmand_new(NOIA_ARGMAND_MOVE, 0));
}

//------------------------------------------------------------------------------

void noia_put_resize(NoiaList* stack)
{
    noia_list_append(stack, noia_argmand_new(NOIA_ARGMAND_RESIZE, 0));
}

//------------------------------------------------------------------------------

void noia_put_number(NoiaList* stack, int code,
                     NOIA_UNUSED uint32_t modifiers,
                     NOIA_UNUSED NoiaKeyState state)
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

    if (noia_list_len(stack) != 0
    && ((NoiaArgmand*) noia_list_last(stack)->data)->type
    == NOIA_ARGMAND_NUMBER) {
        NoiaArgmand* argmand = (NoiaArgmand*) noia_list_last(stack)->data;
        argmand->value *= number;
    } else {
        noia_list_append(stack, noia_argmand_new(NOIA_ARGMAND_NUMBER, number));
    }
}

//------------------------------------------------------------------------------

void noia_anchorize(NoiaList* stack)
{
    noia_list_clean(stack);
    noia_exhibitor_command_anchorize();
}

//------------------------------------------------------------------------------

static inline void noia_position_change(NoiaList* stack,
                                        NoiaArgmandType direction)
{
    int magnitude = 1;
    while (noia_list_len(stack)) {
        NoiaArgmand* argmand = (NoiaArgmand*) noia_list_pop(stack);

        if (argmand->type == NOIA_ARGMAND_FOCUS
        ||  argmand->type == NOIA_ARGMAND_MOVE
        ||  argmand->type == NOIA_ARGMAND_RESIZE) {
            noia_exhibitor_command_position(argmand->type,
                                            direction, magnitude);
            magnitude = 1;
        } else if (argmand->type == NOIA_ARGMAND_NUMBER) {
            magnitude = argmand->value;
        }

        noia_argmand_free(argmand);
    }
}

//------------------------------------------------------------------------------

void noia_right(NoiaList* stack)
{
    noia_position_change(stack, NOIA_ARGMAND_E);
}

//------------------------------------------------------------------------------

void noia_left(NoiaList* stack)
{
    noia_position_change(stack, NOIA_ARGMAND_W);
}

//------------------------------------------------------------------------------

void noia_up(NoiaList* stack)
{
    noia_position_change(stack, NOIA_ARGMAND_N);
}

//------------------------------------------------------------------------------

void noia_down(NoiaList* stack)
{
    noia_position_change(stack, NOIA_ARGMAND_S);
}

//------------------------------------------------------------------------------

