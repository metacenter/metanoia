// file: device-common.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_DEVICE_COMMON_H
#define NOIA_DEVICE_COMMON_H

#include <fcntl.h> // for flag definitions

/// Try to open device.
/// If do not have permissions, try to take device from `logind` using dbus.
int noia_device_open(const char* node, int flags);

#endif // NOIA_DEVICE_COMMON_H

