// file: consfiguration.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONFIGURATION_H__
#define __AURA_CONFIGURATION_H__

#include "global-functions.h"

#include <linux/input.h>

static const AuraConfig scConfig = {
        .run_in_test_window = 1, // TODO: this should be detected in runtime
    };

static const Binding scKeyboardBindings[] = {
        {KEY_ESC, AURA_KEY_CTRL | AURA_KEY_ALT, aura_quit},
        {0}
    };

#endif // __AURA_CONFIGURATION_H__

