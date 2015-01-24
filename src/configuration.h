// file: consfiguration.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONFIGURATION_H__
#define __AURA_CONFIGURATION_H__

#include "global-functions.h"
#include "keyboard-functions.h"
#include "keyboard-binding.h"

#include <linux/input.h>

static const AuraConfig scConfig = {
        .dummy = 0,
    };

static const AuraBinding scBindings[] = {
        // general
        {KEY_ESC,  AURA_KEY_CTRL | AURA_KEY_ALT,  {aura_quit}},
        {KEY_F1,   AURA_KEY_CTRL | AURA_KEY_ALT,  {aura_refresh_displays}},
        {KEY_TAB,  AURA_KEY_ALT,                  {aura_cicle_history_forward}},
        {KEY_TAB,  AURA_KEY_ALT | AURA_KEY_SHIFT, {aura_cicle_history_back}},
        // stack
        {KEY_ESC,   0, {aura_clean_stack}},
        {KEY_F,     0, {aura_put_focus}},
        {KEY_M,     0, {aura_put_move}},
        {KEY_R,     0, {aura_put_resize}},
        {KEY_RIGHT, 0, {aura_right}},
        {KEY_LEFT,  0, {aura_left}},
        {KEY_UP,    0, {aura_up}},
        {KEY_DOWN,  0, {aura_down}},
        // numbers
        {KEY_MINUS,     0, {.execute = aura_put_number}},
        {KEY_0,         0, {.execute = aura_put_number}},
        {KEY_1,         0, {.execute = aura_put_number}},
        {KEY_2,         0, {.execute = aura_put_number}},
        {KEY_3,         0, {.execute = aura_put_number}},
        {KEY_4,         0, {.execute = aura_put_number}},
        {KEY_5,         0, {.execute = aura_put_number}},
        {KEY_6,         0, {.execute = aura_put_number}},
        {KEY_7,         0, {.execute = aura_put_number}},
        {KEY_8,         0, {.execute = aura_put_number}},
        {KEY_9,         0, {.execute = aura_put_number}},
        {KEY_NUMERIC_0, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_1, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_2, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_3, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_4, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_5, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_6, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_7, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_8, 0, {.execute = aura_put_number}},
        {KEY_NUMERIC_9, 0, {.execute = aura_put_number}},
    };

#endif // __AURA_CONFIGURATION_H__

