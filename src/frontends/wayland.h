// file: wayland.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_H__
#define __AURA_WAYLAND_H__

#include "utils/event_dispatcher.h"

// TODO: tmp
#include "surface-priv.h"
struct wl_display* get_wayland_display();

void aura_wayland_initialize(void);
void aura_wayland_notify_frame(SurfaceData* surface);

#endif // __AURA_WAYLAND_H__
