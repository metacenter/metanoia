// file: dbus.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_DBUS_H__
#define __AURA_DBUS_H__

#include <stdint.h>
#include <stdbool.h>

bool aura_dbus_initalize(void);
void aura_dbus_finalize(void);

int aura_dbus_session_get_session_by_pid(int pid, const char** sid);

int aura_dbus_session_take_control();
int aura_dbus_session_release_control();

int aura_dbus_session_take_device(uint32_t major, uint32_t minor);

#endif // __AURA_DBUS_H__

