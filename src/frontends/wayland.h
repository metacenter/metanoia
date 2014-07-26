// file: wayland.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_H__
#define __AURA_WAYLAND_H__

#include "utils/event_dispatcher.h"

// TODO: tmp
struct wl_display* get_wayland_display();

void aura_wayland_initialize(void);

#endif // __AURA_WAYLAND_H__
