// file: device-common.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_DEVICE_COMMON_H__
#define __NOIA_DEVICE_COMMON_H__

#include <fcntl.h> // for flag definitions

int noia_open(const char* node, int flags);

#endif // __NOIA_DEVICE_COMMON_H__

