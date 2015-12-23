// file: config.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_CONFIG_H
#define NOIA_CONFIG_H

#include "utils-keymap.h"
#include "global-constants.h"

/// Keeps information about internal application configuration.
/// @see NoiaSettings
typedef struct {
    const char* background_image_path;
    NoiaBGTransform background_image_transform;
    NoiaColor background_color;
} NoiaConfig;

/// Keeps information about external environment configuration.
/// @see NoiaConfig
typedef struct {
    bool run_in_test_mode;
    bool use_gl;
    bool use_drm;
    char* wayland_display_name;
} NoiaSettings;

/// Get configuration.
const NoiaConfig* noia_config(void);

/// Get settings.
const NoiaSettings* noia_settings(void);

/// Apply configuration:
///
/// -# load settings and configuration
/// -# read environment variables
/// -# parse program arguments
/// -# log all settings
void noia_config_apply(int argc, char** argv);

/// Free memory allocated for storing configuration.
void noia_config_finalize(void);

/// Get key map.
NoiaKeymap* noia_config_get_keymap(void);

#endif // NOIA_CONFIG_H

