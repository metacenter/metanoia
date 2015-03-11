// file: utils-environment.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_ENVIRONMENT_H__
#define __NOIA_UTILS_ENVIRONMENT_H__

#include "global-constants.h"

#define __USE_GNU
#define _GNU_SOURCE
#include <pthread.h>

typedef enum NoiaFilePath {
    RUNTIME_PATH,
    DATA_PATH
} NoiaPath;

void noia_environment_block_system_signals(void);

void noia_environment_unblock_system_signals(void);

void noia_environment_set_thread_name(pthread_t, char*);

void noia_environment_on_enter_new_thread(pthread_t, char*);

int noia_environment_setup(void);

void noia_environment_cleanup(void);

int noia_environment_open_file(const char *file_name,
                               unsigned size,
                               NoiaPath path);

#endif // __NOIA_UTILS_ENVIRONMENT_H__
