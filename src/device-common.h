// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_COMMON_H
#define NOIA_DEVICE_COMMON_H

#include <fcntl.h> // for flag definitions

/// Try to open device.
/// If do not have permissions, try to take device from `logind` using dbus.
int noia_device_open(const char* node, int flags);

#endif // NOIA_DEVICE_COMMON_H

