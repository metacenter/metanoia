// file: utils-dbus.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_DBUS_H__
#define __NOIA_UTILS_DBUS_H__

#include <stdint.h>
#include <stdbool.h>

bool noia_dbus_initalize(void);
void noia_dbus_finalize(void);

int noia_dbus_session_get_session_by_pid(int pid, const char** sid);

int noia_dbus_session_take_control(void);
int noia_dbus_session_release_control(void);

int noia_dbus_session_take_device(uint32_t major, uint32_t minor);

#endif // __NOIA_UTILS_DBUS_H__

