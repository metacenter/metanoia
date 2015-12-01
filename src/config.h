// file: config.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_CONFIG_H__
#define __NOIA_CONFIG_H__

#include "utils-keymap.h"
#include "global-constants.h"

/// Keeps information about internal application configuration.
/// @see NoiaSettings
typedef struct {
    int dummy;
} NoiaConfig;

/// Keeps information about external environment configuration.
/// @see NoiaConfig
typedef struct {
    bool run_in_test_mode;
    bool use_gl;
    bool use_drm;
    const char* wayland_display_name;
} NoiaSettings;

/// Get configuration.
const NoiaConfig* noia_config();

/// Get settings.
const NoiaSettings* noia_settings();

/// Apply configuration:
///
/// -# load settings and configuration
/// -# read environment variables
/// -# parse program arguments
/// -# log all settings
void noia_config_apply(int argc, char** argv);

/// Free memory allocated for storing configuration.
void noia_config_finalize();

/// Get key map.
NoiaKeymap* noia_config_get_keymap();

#endif // __NOIA_CONFIG_H__

