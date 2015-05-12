// file: config.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "config.h"
#include "configuration.h"
#include "keyboard-bindings.h"
#include "utils-log.h"

#include <stdlib.h>
#include <memory.h>

static const char* scDefaultWaylandDisplayName = "wayland-0";

NoiaKeymap* keymap;

static NoiaSettings sSettings;

//------------------------------------------------------------------------------

void noia_config_apply()
{
    memset(&sSettings, 0, sizeof(NoiaSettings));

    // Apply keybinding config
    keymap = noia_utils_keymap_new();
    noia_utils_keymap_initialize(keymap);

    for (unsigned i = 0; i < sizeof(scBindings)/sizeof(NoiaBinding); ++i) {
        noia_keyboard_add_binding(NOIA_NORMAL_MODE, &scBindings[i]);
    }

    // Apply evironment variables
    sSettings.run_in_test_window = (getenv("DISPLAY") != NULL);
    sSettings.wayland_display_name = strdup(getenv("METANOIA_WAYLAND_DISPLAY"));
    if (sSettings.wayland_display_name == NULL) {
        sSettings.wayland_display_name = scDefaultWaylandDisplayName;
    }
}

//------------------------------------------------------------------------------

void noia_config_finalize()
{
    noia_utils_keymap_finalize(keymap);
    noia_utils_keymap_free(keymap);

    noia_keyboard_free_all();
}

//------------------------------------------------------------------------------

NoiaKeymap* noia_config_get_keymap()
{
    return keymap;
}

//------------------------------------------------------------------------------

const NoiaConfig* noia_config()
{
    return &scConfig;
}

//------------------------------------------------------------------------------

const NoiaSettings* noia_settings()
{
    return &sSettings;
}

//------------------------------------------------------------------------------

