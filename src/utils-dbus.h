// file: utils-dbus.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_UTILS_DBUS_H
#define NOIA_UTILS_DBUS_H

#include "global-constants.h"

NoiaResult noia_dbus_initalize(void);
void noia_dbus_finalize(void);

NoiaResult noia_dbus_session_get_session_by_pid(int pid, const char** sid);

NoiaResult noia_dbus_session_take_control(void);
NoiaResult noia_dbus_session_release_control(void);

NoiaResult noia_dbus_session_take_device(uint32_t major, uint32_t minor);

#endif // NOIA_UTILS_DBUS_H

