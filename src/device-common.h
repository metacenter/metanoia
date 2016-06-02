// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_COMMON_H
#define NOIA_DEVICE_COMMON_H

#include <fcntl.h> // for flag definitions
#include <stddef.h>

/// UDev action names.
extern const char* scActionAdd;
extern const char* scActionRemove;

/// UDev property names.
extern const char* scIdInputMouse;
extern const char* scIdInputTouchpad;
extern const char* scIdInputTablet;
extern const char* scIdInputTouchscreen;
extern const char* scIdInputJoystic;
extern const char* scIdInputKey;
extern const char* scIdInputKeyboard;

/// Input device property values
typedef enum {
    NOIA_PROPERTY_MOUSE       = 0x0001,
    NOIA_PROPERTY_TOUCHPAD    = 0x0002,
    NOIA_PROPERTY_TABLET      = 0x0004,
    NOIA_PROPERTY_TOUCHSCREEN = 0x0008,
    NOIA_PROPERTY_JOYSTICK    = 0x0010,
    NOIA_PROPERTY_KEY         = 0x0020,
    NOIA_PROPERTY_KEYBOARD    = 0x0040,
} NoiaInputDeviceProperty;

/// Try to open device.
/// If do not have permissions, try to take device from `logind` using dbus.
int noia_device_open(const char* node, int flags);

/// Get name of device.
/// Name will be written to buffer `buff` o given size `size`.
void noia_device_get_name(int fd, char* buff, size_t size);

#endif // NOIA_DEVICE_COMMON_H

