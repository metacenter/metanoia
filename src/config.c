// file: config.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "config.h"
#include "configuration.h"
#include "keyboard-bindings.h"
#include "utils-log.h"

#include <stdlib.h>
#include <memory.h>

// FIXME: tmp
AuraKeymap* keymap;

static AuraSettings sSettings;

//------------------------------------------------------------------------------

void aura_config_apply()
{
    memset(&sSettings, 0, sizeof(AuraSettings));

    // Apply keybinding config
    keymap = aura_utils_keymap_new();
    aura_utils_keymap_initialize(keymap);

    unsigned int i;
    for (i=0; i < sizeof(scArgmands)/sizeof(AuraArgmand)-1; ++i) {
        aura_keyboard_add_argmand(AURA_NORMAL_MODE, &scArgmands[i]);
    }

    // Apply evironment variables
    sSettings.run_in_test_window = (getenv("DISPLAY") != NULL);
}

//------------------------------------------------------------------------------

AuraKeymap* aura_config_get_keymap()
{
    return keymap;
}

//------------------------------------------------------------------------------

const AuraConfig* aura_config()
{
    return &scConfig;
}

//------------------------------------------------------------------------------

const AuraSettings* aura_settings()
{
    return &sSettings;
}

//------------------------------------------------------------------------------

