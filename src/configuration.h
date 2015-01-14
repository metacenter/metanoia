// file: consfiguration.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONFIGURATION_H__
#define __AURA_CONFIGURATION_H__

#include "global-functions.h"
#include "keyboard-argmand.h"

#include <linux/input.h>

static const AuraConfig scConfig = {
        .dummy = 0,
    };

static const AuraArgmand scArgmands[] = {
        {KEY_ESC,   AURA_KEY_CTRL | AURA_KEY_ALT,   aura_quit},
        {KEY_F1,    AURA_KEY_CTRL | AURA_KEY_ALT,   aura_refresh_displays},
        {KEY_TAB,   AURA_KEY_ALT,                   aura_cicle_history_forward},
        {KEY_TAB,   AURA_KEY_ALT  | AURA_KEY_SHIFT, aura_cicle_history_back},
        {KEY_ESC,   0,                              aura_clean_stack},
        {KEY_F,     0,                              aura_put_focus},
        {KEY_RIGHT, 0,                              aura_right},
        {KEY_LEFT,  0,                              aura_left},
        {KEY_UP,    0,                              aura_up},
        {KEY_DOWN,  0,                              aura_down},
    };

#endif // __AURA_CONFIGURATION_H__

