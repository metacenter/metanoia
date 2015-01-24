// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-functions.h"
#include "keyboard-argmand.h"
#include "utils-log.h"
#include "exhibitor.h"

#include <linux/input.h>

//------------------------------------------------------------------------------

void aura_clean_stack(Chain* stack)
{
    chain_clean(stack);
}

//------------------------------------------------------------------------------

void aura_put_focus(Chain* stack)
{
    chain_append(stack, aura_argmand_new(AURA_ARGMAND_FOCUS, 0));
}

//------------------------------------------------------------------------------

void aura_put_move(Chain* stack)
{
    chain_append(stack, aura_argmand_new(AURA_ARGMAND_MOVE, 0));
}

//------------------------------------------------------------------------------

void aura_put_resize(Chain* stack)
{
    chain_append(stack, aura_argmand_new(AURA_ARGMAND_RESIZE, 0));
}

//------------------------------------------------------------------------------

void aura_put_number(Chain* stack, int code,
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

    if (chain_len(stack) != 0
    && ((AuraArgmand*) stack->last->data)->type == AURA_ARGMAND_NUMBER) {
        AuraArgmand* argmand = (AuraArgmand*) stack->last->data;
        argmand->value *= number;
    } else {
        chain_append(stack, aura_argmand_new(AURA_ARGMAND_NUMBER, number));
    }
}

//------------------------------------------------------------------------------

static inline void aura_position_change(Chain* stack, AuraArgmandType direction)
{
    int magnitude = 1;
    while (chain_len(stack)) {
        AuraArgmand* argmand = (AuraArgmand*) chain_pop(stack);

        if (argmand->type == AURA_ARGMAND_FOCUS
        ||  argmand->type == AURA_ARGMAND_MOVE
        ||  argmand->type == AURA_ARGMAND_RESIZE) {
            aura_exhibitor_command_position(argmand->type,
                                            direction, magnitude);
            magnitude = 1;
        } else if (argmand->type == AURA_ARGMAND_NUMBER) {
            magnitude = argmand->value;
        }
    }
}

//------------------------------------------------------------------------------

void aura_right(Chain* stack)
{
    aura_position_change(stack, AURA_ARGMAND_E);
}

//------------------------------------------------------------------------------

void aura_left(Chain* stack)
{
    aura_position_change(stack, AURA_ARGMAND_W);
}

//------------------------------------------------------------------------------

void aura_up(Chain* stack)
{
    aura_position_change(stack, AURA_ARGMAND_N);
}

//------------------------------------------------------------------------------

void aura_down(Chain* stack)
{
    aura_position_change(stack, AURA_ARGMAND_S);
}

//------------------------------------------------------------------------------

