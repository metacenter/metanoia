// file: config.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "config.h"
#include "configuration.h"
#include "keyboard-bindings.h"
#include "utils-log.h"

#include <stdlib.h>
#include <memory.h>

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
    for (i=0; i < sizeof(scBindings)/sizeof(AuraBinding); ++i) {
        aura_keyboard_add_binding(AURA_NORMAL_MODE, &scBindings[i]);
    }

    // Apply evironment variables
    sSettings.run_in_test_window = (getenv("DISPLAY") != NULL);
}

//------------------------------------------------------------------------------

void aura_config_finalize()
{
    aura_utils_keymap_finalize(keymap);
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

