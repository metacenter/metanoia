// file: config.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONFIG_H__
#define __AURA_CONFIG_H__

#include "utils-keymap.h"
#include "global-constants.h"

typedef struct AuraConfig {
    bool run_in_test_window;
} AuraConfig;

AuraConfig aura_config();

void aura_config_apply();

AuraKeymap* aura_config_get_keymap();

#endif // __AURA_CONFIG_H__

