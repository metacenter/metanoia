// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-functions.h"
#include "utils-log.h"
#include "exhibitor.h"

//------------------------------------------------------------------------------

void aura_clean_stack(Chain* stack)
{
    chain_clean(stack);
}

//------------------------------------------------------------------------------

void aura_put_focus(Chain* stack)
{
    chain_append(stack, (void*) AURA_ARGMAND_FOCUS);
}

//------------------------------------------------------------------------------

void aura_put_move(Chain* stack)
{
    chain_append(stack, (void*) AURA_ARGMAND_MOVE);
}

//------------------------------------------------------------------------------

void aura_put_resize(Chain* stack)
{
    chain_append(stack, (void*) AURA_ARGMAND_RESIZE);
}

//------------------------------------------------------------------------------

static inline void aura_position_change(Chain* stack, AuraArgmandType direction)
{
    while (chain_len(stack)) {
        AuraArgmandType type = (AuraArgmandType) chain_pop(stack);
        if (type == AURA_ARGMAND_FOCUS
        ||  type == AURA_ARGMAND_MOVE
        ||  type == AURA_ARGMAND_RESIZE) {
            aura_exhibitor_command_position(type, direction, 1);
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

