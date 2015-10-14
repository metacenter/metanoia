// file: config.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_CONFIG_H__
#define __NOIA_CONFIG_H__

#include "utils-keymap.h"
#include "global-constants.h"

typedef struct {
    int dummy;
} NoiaConfig;

typedef struct {
    bool run_in_test_window;
    bool use_gl;
    bool use_drm;
    const char* wayland_display_name;
} NoiaSettings;

const NoiaConfig* noia_config();
const NoiaSettings* noia_settings();

void noia_config_apply(int argc, char** argv);
void noia_config_finalize();

NoiaKeymap* noia_config_get_keymap();

#endif // __NOIA_CONFIG_H__

