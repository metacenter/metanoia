// file: wayland.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_WAYLAND_H__
#define __NOIA_WAYLAND_H__

#include "event-loop.h"

struct wl_display* get_wayland_display();

void noia_wayland_initialize(NoiaLoop* this_loop);

#endif // __NOIA_WAYLAND_H__
