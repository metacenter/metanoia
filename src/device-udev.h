// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_UDEV_H
#define NOIA_DEVICE_UDEV_H

#include "event-dispatcher.h"

/// Set up device monitoring.
/// Subscribe for dev types "input" and "drm".
void noia_udev_setup_device_monitoring(NoiaEventDispatcher* ed);

/// Find input devices using udev.
void noia_udev_setup_input_devices(NoiaEventDispatcher* ed);

#endif // NOIA_DEVICE_UDEV_H

