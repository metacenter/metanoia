// file: config.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONFIG_H__
#define __AURA_CONFIG_H__

#include "utils-keymap.h"
#include "global-constants.h"

typedef struct {
} AuraConfig;

typedef struct {
    bool run_in_test_window;
} AuraSettings;

const AuraConfig aura_config();
const AuraSettings aura_settings();

void aura_config_apply();

AuraKeymap* aura_config_get_keymap();

#endif // __AURA_CONFIG_H__

