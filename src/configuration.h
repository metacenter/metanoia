// file: consfiguration.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONFIGURATION_H__
#define __AURA_CONFIGURATION_H__

#include "global-functions.h"
#include "keyboard-argmand.h"

#include <linux/input.h>

static const AuraConfig scConfig = {
    };

static const AuraArgmand scArgmands[] = {
        {KEY_ESC, AURA_KEY_CTRL | AURA_KEY_ALT,   aura_quit},
        {KEY_F1,  AURA_KEY_CTRL | AURA_KEY_ALT,   aura_refresh_displays},
        {KEY_TAB, AURA_KEY_ALT,                   aura_cicle_history_forward},
        {KEY_TAB, AURA_KEY_ALT  | AURA_KEY_SHIFT, aura_cicle_history_back},
        {0}
    };

#endif // __AURA_CONFIGURATION_H__

