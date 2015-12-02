// file: wayland.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_H
#define NOIA_WAYLAND_H

#include "event-loop.h"

struct wl_display* get_wayland_display(void);

void noia_wayland_initialize(NoiaLoop* this_loop);

#endif // NOIA_WAYLAND_H
