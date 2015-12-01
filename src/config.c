// file: config.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "config.h"
#include "configuration.h"
#include "keyboard-bindings.h"
#include "global-macros.h"
#include "utils-log.h"

#include <stdlib.h>
#include <memory.h>
#include <argp.h>

static const char* scDefaultWaylandDisplayName = "wayland-0";

/// @todo Don't use global variables
NoiaKeymap* keymap;

static NoiaSettings sSettings;

//------------------------------------------------------------------------------

int noia_config_parse_option(int key,
                             char *arg NOIA_UNUSED,
                             struct argp_state* state)
{
    NoiaSettings* settings = state->input;

    switch (key) {
        case 'g': settings->use_gl = false; break;
        case 'd': settings->use_drm = false; break;
        case ARGP_KEY_ARG: return 0;
        default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

//------------------------------------------------------------------------------

void noia_config_parse_arguments(int argc, char** argv)
{
    struct argp_option options[] = {
            { "no-gl",  'g', 0, 0, "do not use gl for drawing", 0},
            { "no-drm", 'd', 0, 0, "do not use DRM do device lookup", 0},
            { NULL, '\0', 0, 0, NULL, 0 }
        };
    struct argp argp = {options, noia_config_parse_option,
                        NULL, NULL, NULL, NULL, NULL};

    argp_parse(&argp, argc, argv, 0, 0, &sSettings);
}

//------------------------------------------------------------------------------

void noia_config_log_settings()
{
    LOG_INFO2("Settings: run in test mode = '%d'",
              sSettings.run_in_test_mode);
    LOG_INFO2("Settings: use GL = '%d'",
              sSettings.use_gl);
    LOG_INFO2("Settings: use DRM = '%d'",
              sSettings.use_drm);
    LOG_INFO2("Settings: wayland display name = '%s'",
              sSettings.wayland_display_name);
}

//------------------------------------------------------------------------------

void noia_config_apply(int argc, char** argv)
{
    memset(&sSettings, 0, sizeof(NoiaSettings));
    sSettings.use_gl = true;
    sSettings.use_drm = true;

    // Apply keybinding config
    keymap = noia_utils_keymap_new();
    noia_utils_keymap_initialize(keymap);

    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(scBindingsCommon); ++i) {
        noia_keyboard_add_binding(NOIA_MODE_COMMON, &scBindingsCommon[i]);
    }
    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(scBindingsNormal); ++i) {
        noia_keyboard_add_binding(NOIA_MODE_NORMAL, &scBindingsNormal[i]);
    }
    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(scBindingsInsert); ++i) {
        noia_keyboard_add_binding(NOIA_MODE_INSERT, &scBindingsInsert[i]);
    }
    noia_keyboard_mode_make_active(NOIA_MODE_COMMON, true);
    noia_keyboard_mode_make_active(NOIA_MODE_NORMAL, true);

    // Apply evironment variables
    sSettings.run_in_test_mode = (getenv("DISPLAY") != NULL);

    const char* env = getenv("METANOIA_WAYLAND_DISPLAY");
    if (env) {
        sSettings.wayland_display_name = strdup(env);
    } else {
        sSettings.wayland_display_name = scDefaultWaylandDisplayName;
    }

    // Parse program arguments
    noia_config_parse_arguments(argc, argv);

    // Log all setting
    noia_config_log_settings();
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

