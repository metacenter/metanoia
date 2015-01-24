// file: utils-environment.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_ENVIRONMENT_H__
#define __AURA_UTILS_ENVIRONMENT_H__

#include "global-constants.h"

#define __USE_GNU
#define _GNU_SOURCE
#include <pthread.h>

typedef enum AuraFilePath {
    RUNTIME_PATH,
    DATA_PATH
} AuraPath;

void aura_environment_block_system_signals(void);

void aura_environment_unblock_system_signals(void);

void aura_environment_set_thread_name(pthread_t, char*);

void aura_environment_on_enter_new_thread(pthread_t, char*);

int aura_environment_setup(void);

void aura_environment_cleanup(void);

int aura_environment_open_file(const char *file_name,
                               unsigned size,
                               AuraPath path);

#endif // __AURA_UTILS_ENVIRONMENT_H__
