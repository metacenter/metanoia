// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_DBUS_H
#define NOIA_UTILS_DBUS_H

#include "global-constants.h"

NoiaResult noia_dbus_initalize(void);
void noia_dbus_finalize(void);

NoiaResult noia_dbus_session_get_session_by_pid(int pid, const char** sid);

NoiaResult noia_dbus_session_take_control(void);
NoiaResult noia_dbus_session_release_control(void);

int noia_dbus_session_take_device(uint32_t major, uint32_t minor);

#endif // NOIA_UTILS_DBUS_H

