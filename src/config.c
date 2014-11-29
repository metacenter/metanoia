// file: config.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "configuration.h"
#include "keyboard-bindings.h"
#include "utils-log.h"
#include "utils-keymap.h"

// FIXME: tmp
AuraKeymap* keymap;

//------------------------------------------------------------------------------

void aura_config_apply()
{
    keymap = aura_utils_keymap_new();
    aura_utils_keymap_initialize(keymap);

    int i;
    for (i=0; i < sizeof(scKeyboardBindings)/sizeof(Binding)-1; ++i) {
        aura_keyboard_add_binding(&scKeyboardBindings[i]);
    }
}

//------------------------------------------------------------------------------

