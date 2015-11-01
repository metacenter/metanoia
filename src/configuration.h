// file: consfiguration.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_CONFIGURATION_H__
#define __NOIA_CONFIGURATION_H__

#include "global-functions.h"
#include "keyboard-functions.h"
#include "keyboard-binding.h"

#include <linux/input.h>

static const NoiaConfig scConfig = {
        .dummy = 0,
    };

static const NoiaBinding scBindings[] = {
        // general
        {KEY_ESC,  NOIA_KEY_CTRL|NOIA_KEY_ALT,   {noia_quit}},
        {KEY_F1,   NOIA_KEY_CTRL|NOIA_KEY_ALT,   {noia_refresh_displays}},
        {KEY_TAB,  NOIA_KEY_META,                {noia_cicle_history_forward}},
        {KEY_TAB,  NOIA_KEY_META|NOIA_KEY_SHIFT, {noia_cicle_history_back}},
        // stack
        {KEY_ESC,   0, {noia_clean_stack}},
        {KEY_F,     0, {noia_put_focus}},
        {KEY_M,     0, {noia_put_move}},
        {KEY_R,     0, {noia_put_resize}},
        {KEY_A,     0, {noia_anchorize}},
        {KEY_RIGHT, 0, {noia_right}},
        {KEY_LEFT,  0, {noia_left}},
        {KEY_UP,    0, {noia_up}},
        {KEY_DOWN,  0, {noia_down}},
        // numbers
        {KEY_MINUS,     0, {.execute = noia_put_number}},
        {KEY_0,         0, {.execute = noia_put_number}},
        {KEY_1,         0, {.execute = noia_put_number}},
        {KEY_2,         0, {.execute = noia_put_number}},
        {KEY_3,         0, {.execute = noia_put_number}},
        {KEY_4,         0, {.execute = noia_put_number}},
        {KEY_5,         0, {.execute = noia_put_number}},
        {KEY_6,         0, {.execute = noia_put_number}},
        {KEY_7,         0, {.execute = noia_put_number}},
        {KEY_8,         0, {.execute = noia_put_number}},
        {KEY_9,         0, {.execute = noia_put_number}},
        {KEY_NUMERIC_0, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_1, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_2, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_3, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_4, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_5, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_6, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_7, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_8, 0, {.execute = noia_put_number}},
        {KEY_NUMERIC_9, 0, {.execute = noia_put_number}},
    };

#endif // __NOIA_CONFIGURATION_H__

