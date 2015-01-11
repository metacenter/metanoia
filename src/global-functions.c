// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "global-functions.h"
#include "utils-log.h"
#include "event-signals.h"
#include "exhibitor.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//------------------------------------------------------------------------------

void aura_quit()
{
    LOG_INFO1("QUIT!");
    kill(getpid(), SIGTERM);
}

//------------------------------------------------------------------------------

void aura_refresh_displays()
{
    LOG_DEBUG("REFRESH");
    aura_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
}

//------------------------------------------------------------------------------

void aura_cicle_history_forward()
{
    LOG_DEBUG("CICLE HISTORY FORWARD");
    aura_exhibitor_pop_history_surface(1);
}

//------------------------------------------------------------------------------

void aura_cicle_history_back()
{
    LOG_DEBUG("CICLE HISTORY BACK");
    aura_exhibitor_pop_history_surface(-1);
}

//------------------------------------------------------------------------------

void aura_clean_stack(Chain* stack)
{
    LOG_DEBUG("CLEAN");
    chain_clean(stack);
}

//------------------------------------------------------------------------------

void aura_put_focus(Chain* stack)
{
    LOG_DEBUG("PUT FOCUS");
    chain_append(stack, (void*) AURA_ARGMAND_FOCUS);
}

//------------------------------------------------------------------------------

void aura_put_move(Chain* stack)
{
    LOG_DEBUG("PUT MOVE");
    chain_append(stack, (void*) AURA_ARGMAND_MOVE);
}

//------------------------------------------------------------------------------

static inline void aura_position_change(Chain* stack, AuraArgmandType direction)
{
    while (chain_len(stack)) {
        AuraArgmandType type = (AuraArgmandType) chain_pop(stack);
        if (type == AURA_ARGMAND_FOCUS
        ||  type == AURA_ARGMAND_MOVE) {
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

