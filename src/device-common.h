// file: device-common.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_DEVICE_COMMON_H__
#define __AURA_DEVICE_COMMON_H__

#include <fcntl.h> // for flag definitions

int aura_open(const char* node, int flags);

#endif // __AURA_DEVICE_COMMON_H__

