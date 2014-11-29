// file: utils-environment.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_ENVIRONMENT_H__
#define __AURA_UTILS_ENVIRONMENT_H__

#include "global-constants.h"

int aura_environment_setup(void);

void aura_environment_cleanup(void);

int aura_environment_open_runtime_file(const char *file_name,
                                       unsigned size);

#endif // __AURA_UTILS_ENVIRONMENT_H__
