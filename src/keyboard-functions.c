// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-functions.h"
#include "keyboard-argmand.h"
#include "utils-log.h"
#include "exhibitor.h"

#include <linux/input.h>

//------------------------------------------------------------------------------

void aura_clean_stack(AuraList* stack)
{
    aura_list_clean(stack);
}

//------------------------------------------------------------------------------

void aura_put_focus(AuraList* stack)
{
    aura_list_append(stack, aura_argmand_new(AURA_ARGMAND_FOCUS, 0));
}

//------------------------------------------------------------------------------

void aura_put_move(AuraList* stack)
{
    aura_list_append(stack, aura_argmand_new(AURA_ARGMAND_MOVE, 0));
}

//------------------------------------------------------------------------------

void aura_put_resize(AuraList* stack)
{
    aura_list_append(stack, aura_argmand_new(AURA_ARGMAND_RESIZE, 0));
}

//------------------------------------------------------------------------------

void aura_put_number(AuraList* stack, int code,
                     AURA_UNUSED uint32_t modifiers,
                     AURA_UNUSED AuraKeyState state)
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

    if (aura_list_len(stack) != 0
    && ((AuraArgmand*) aura_list_last(stack)->data)->type
    == AURA_ARGMAND_NUMBER) {
        AuraArgmand* argmand = (AuraArgmand*) aura_list_last(stack)->data;
        argmand->value *= number;
    } else {
        aura_list_append(stack, aura_argmand_new(AURA_ARGMAND_NUMBER, number));
    }
}

//------------------------------------------------------------------------------

static inline void aura_position_change(AuraList* stack,
                                        AuraArgmandType direction)
{
    int magnitude = 1;
    while (aura_list_len(stack)) {
        AuraArgmand* argmand = (AuraArgmand*) aura_list_pop(stack);

        if (argmand->type == AURA_ARGMAND_FOCUS
        ||  argmand->type == AURA_ARGMAND_MOVE
        ||  argmand->type == AURA_ARGMAND_RESIZE) {
            aura_exhibitor_command_position(argmand->type,
                                            direction, magnitude);
            magnitude = 1;
        } else if (argmand->type == AURA_ARGMAND_NUMBER) {
            magnitude = argmand->value;
        }

        aura_argmand_free(argmand);
    }
}

//------------------------------------------------------------------------------

void aura_right(AuraList* stack)
{
    aura_position_change(stack, AURA_ARGMAND_E);
}

//------------------------------------------------------------------------------

void aura_left(AuraList* stack)
{
    aura_position_change(stack, AURA_ARGMAND_W);
}

//------------------------------------------------------------------------------

void aura_up(AuraList* stack)
{
    aura_position_change(stack, AURA_ARGMAND_N);
}

//------------------------------------------------------------------------------

void aura_down(AuraList* stack)
{
    aura_position_change(stack, AURA_ARGMAND_S);
}

//------------------------------------------------------------------------------

