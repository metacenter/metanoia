// file: shared.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "shared.h"
#include "utils/dbus.h"
#include "utils/log.h"

#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <string.h>
#include <sys/stat.h>

int aura_open(const char* node, int flags)
{
    struct stat st;
    int result, fd;

    fd = open(node, flags);
    if (fd > 0) {
        return fd;
    }
    if (fd < 0 && errno != EPERM && errno != EACCES) {
        return fd;
    }

    result = stat(node, &st);
    if (result >= 0) {
        return aura_dbus_session_take_device(major(st.st_rdev),
                                             minor(st.st_rdev));
    }

    return fd;
}

