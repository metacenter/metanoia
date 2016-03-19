// file: config.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_CONFIG_H
#define NOIA_CONFIG_H

#include "utils-list.h"
#include "utils-keymap.h"
#include "global-constants.h"

/// Keeps information about constant internal application configuration.
/// @see NoiaSettings NoiaGears
typedef struct {
    const char* background_image_path;
    NoiaBGTransform background_image_transform;
    NoiaColor background_color;
} NoiaConfig;

/// Keeps information about variable internal application configuration.
/// @see NoiaConfig NoiaSettings
typedef struct {
    NoiaKeymap* keymap;
    NoiaList* modes;
} NoiaGears;

/// Keeps information about external environment configuration.
/// @see NoiaConfig NoiaGears
typedef struct {
    bool run_in_test_mode;
    bool use_gl;
    bool use_drm;
    char* wayland_display_name;
} NoiaSettings;

/// Get configuration.
const NoiaConfig* noia_config(void);

/// Get gears.
NoiaGears* noia_gears(void);

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

#endif // NOIA_CONFIG_H

