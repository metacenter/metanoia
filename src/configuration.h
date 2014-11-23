// file: consfiguration.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONFIGURATION_H__
#define __AURA_CONFIGURATION_H__

#include "global-constants.h"
#include "configuration-functions.h"

#include <linux/input.h>

static const LogLevel scConfLogLevel = LEVEL_INFO3;//LEVEL_DEBUG;
static const char* scConfLogFile = "/home/user/.local/share/aura/logs/log";

static const Binding scKeyboardBindings[] = {
        {KEY_F7,  AURA_KEY_CTRL | AURA_KEY_ALT, aura_vt_activate_7},
        {KEY_ESC, AURA_KEY_CTRL | AURA_KEY_ALT, aura_quit},
        {0}
    };

#endif // __AURA_CONFIGURATION_H__

