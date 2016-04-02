// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-common.h"
#include "utils-dbus.h"
#include "global-macros.h"

#include <errno.h>
#include <libudev.h>
#include <sys/stat.h>

int noia_device_open(const char* node, int flags)
{
    int fd = -1;

    NOIA_BLOCK {
        fd = open(node, flags);
        if (fd > 0) {
            break;
        }

        if (fd < 0 && errno != EPERM && errno != EACCES) {
           break;
        }

        struct stat st;
        if (stat(node, &st) >= 0) {
            fd = noia_dbus_session_take_device(major(st.st_rdev),
                                               minor(st.st_rdev));
        }
    }

    return fd;
}

