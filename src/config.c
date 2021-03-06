// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "config.h"
#include "configuration.h"
#include "input-mode.h"
#include "input-bindings.h"
#include "global-macros.h"
#include "utils-log.h"

#include <stdlib.h>
#include <memory.h>
#include <argp.h>

static const char* scDefaultWaylandDisplayName = "wayland-0";

/// @todo Don't use global variables
static NoiaGears sGears;
static NoiaSettings sSettings;

//------------------------------------------------------------------------------

int noia_config_parse_option(int key,
                             char* arg NOIA_UNUSED,
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

void noia_config_log_settings(void)
{
    LOG_CONFG("Settings: run in test mode = '%d'",
              sSettings.run_in_test_mode);
    LOG_CONFG("Settings: use GL = '%d'",
              sSettings.use_gl);
    LOG_CONFG("Settings: use DRM = '%d'",
              sSettings.use_drm);
    LOG_CONFG("Settings: wayland display name = '%s'",
              sSettings.wayland_display_name);
}

//------------------------------------------------------------------------------

void noia_config_apply(int argc, char** argv)
{
    // Set up settings
    memset(&sSettings, 0, sizeof(NoiaSettings));
    sSettings.use_gl = true;
    sSettings.use_drm = true;

    // Setup gears
    sGears.keymap = noia_utils_keymap_new();
    noia_utils_keymap_initialize(sGears.keymap);

    sGears.modes = noia_list_new((NoiaFreeFunc) noia_mode_destroy);

    NoiaMode* common_mode = noia_mode_create(NOIA_MODE_COMMON, true, NULL);
    NoiaMode* insert_mode = noia_mode_create(NOIA_MODE_INSERT, true, NULL);
    NoiaMode* normal_mode = noia_mode_create(NOIA_MODE_NORMAL, false,
                                             noia_swallow);

    noia_list_append(sGears.modes, common_mode);
    noia_list_append(sGears.modes, insert_mode);
    noia_list_append(sGears.modes, normal_mode);

    // Apply binding config
    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(scBindingsCommon); ++i) {
        noia_mode_add_binding(common_mode, &scBindingsCommon[i]);
    }
    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(scBindingsInsert); ++i) {
        noia_mode_add_binding(insert_mode, &scBindingsInsert[i]);
    }
    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(scBindingsNormal); ++i) {
        noia_mode_add_binding(normal_mode, &scBindingsNormal[i]);
    }

    // Apply evironment variables
    sSettings.run_in_test_mode = (getenv("DISPLAY") != NULL);

    const char* env = getenv("METANOIA_WAYLAND_DISPLAY");
    if (env) {
        sSettings.wayland_display_name = strdup(env);
    } else {
        sSettings.wayland_display_name = strdup(scDefaultWaylandDisplayName);
    }

    // Parse program arguments
    noia_config_parse_arguments(argc, argv);

    // Log all setting
    noia_config_log_settings();
}

//------------------------------------------------------------------------------

void noia_config_finalize(void)
{
    noia_list_free(sGears.modes);

    noia_utils_keymap_finalize(sGears.keymap);
    noia_utils_keymap_free(sGears.keymap);

    if (sSettings.wayland_display_name) {
        free(sSettings.wayland_display_name);
        sSettings.wayland_display_name = NULL;
    }
}

//------------------------------------------------------------------------------

const NoiaConfig* noia_config(void)
{
    return &scConfig;
}

//------------------------------------------------------------------------------

const NoiaSettings* noia_settings(void)
{
    return &sSettings;
}

//------------------------------------------------------------------------------

NoiaGears* noia_gears(void)
{
    return &sGears;
}

//------------------------------------------------------------------------------

