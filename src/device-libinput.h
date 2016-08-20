// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_LIBINPUT_H
#define NOIA_DEVICE_LIBINPUT_H

#include "event-dispatcher.h"

///
void noia_libinput_add_input_device(NoiaEventDispatcher* ed,
                                    const char* devnode,
                                    const char* sysname,
                                    uint32_t properties);

#endif // NOIA_DEVICE_LIBINPUT_H

