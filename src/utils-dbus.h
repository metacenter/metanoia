// file: utils-dbus.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_DBUS_H__
#define __NOIA_UTILS_DBUS_H__

#include "global-constants.h"

#include <stdint.h>
#include <stdbool.h>

NoiaResult noia_dbus_initalize(void);
void noia_dbus_finalize(void);

NoiaResult noia_dbus_session_get_session_by_pid(int pid, const char** sid);

NoiaResult noia_dbus_session_take_control(void);
NoiaResult noia_dbus_session_release_control(void);

NoiaResult noia_dbus_session_take_device(uint32_t major, uint32_t minor);

#endif // __NOIA_UTILS_DBUS_H__

