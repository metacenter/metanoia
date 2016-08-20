// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-common.h"
#include "utils-dbus.h"
#include "global-macros.h"

#include <linux/input.h>
#include <sys/stat.h>

#include <errno.h>
#include <string.h>

//------------------------------------------------------------------------------

const char* scActionAdd    = "add";
const char* scActionRemove = "remove";

const char* scIdInputMouse       = "ID_INPUT_MOUSE";
const char* scIdInputTouchpad    = "ID_INPUT_TOUCHPAD";
const char* scIdInputTablet      = "ID_INPUT_TABLET";
const char* scIdInputTouchscreen = "ID_INPUT_TOUCHSCREEN";
const char* scIdInputJoystic     = "ID_INPUT_JOYSTICK";
const char* scIdInputKey         = "ID_INPUT_KEY";
const char* scIdInputKeyboard    = "ID_INPUT_KEYBOARD";

//------------------------------------------------------------------------------

int noia_device_open(const char* node, int flags)
{
    int fd = -1;

    NOIA_BLOCK {
        fd = open(node, flags);
        if (fd > 0) {
            break;
        }

        if ((fd < 0) and (errno != EPERM) and (errno != EACCES)) {
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

//------------------------------------------------------------------------------

void noia_device_get_name(int fd, char* buff, size_t size)
{
    ioctl(fd, EVIOCGNAME(size), buff);
}

//------------------------------------------------------------------------------

bool noia_device_is_event(const char* devnode, const char* sysname)
{
    bool result = false;
    NOIA_BLOCK {
        // Does it exist and is of correct type?
        struct stat st;
        if ((not devnode)
        or  (stat(devnode, &st) < 0)
        or  (not S_ISCHR(st.st_mode))) {
            break;
        }

        // Is it event device?
        if (strncmp("event", sysname, 5) != 0) {
            break;
        }

        // Found event input device
        result = true;
    }
    return result;
}

//------------------------------------------------------------------------------

