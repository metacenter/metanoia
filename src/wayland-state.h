// file: wayland-state.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_STATE_H
#define NOIA_WAYLAND_STATE_H

#include "utils-store.h"
#include "output.h"
#include "wayland-surface.h"

#include <wayland-server.h>

NoiaResult noia_wayland_state_initialize(struct wl_display* display);

void noia_wayland_state_finalize(void);

void noia_wayland_state_add_surface(NoiaItemId sid, struct wl_resource* rc);

void noia_wayland_state_remove_surface(NoiaItemId sid);

void noia_wayland_state_surface_attach(NoiaItemId sid, struct wl_resource* rc);

void noia_wayland_state_subscribe_frame(NoiaItemId sid, struct wl_resource* rc);

void noia_wayland_state_add_keyboard_resource(struct wl_resource* rc);

void noia_wayland_state_keyboard_focus_update(NoiaItemId sid);

void noia_wayland_state_key(uint32_t time, uint32_t key, uint32_t state);

void noia_wayland_state_pointer_focus_update(NoiaSurfaceId new_sid,
                                             NoiaPosition pos);

void noia_wayland_state_pointer_motion(NoiaSurfaceId sid, NoiaPosition pos);

void noia_wayland_state_pointer_button(uint32_t time,
                                       uint32_t button,
                                       uint32_t state);

void noia_wayland_state_set_cursor(int serial,
                                   int hotspot_x,
                                   int hotspot_y,
                                   NoiaSurfaceId sid);

void noia_wayland_state_screen_refresh(NoiaSurfaceId sid);

void noia_wayland_state_advertise_output(NoiaOutput* output);

void noia_wayland_state_destroy_output(NoiaOutput* output);

void noia_wayland_state_surface_reconfigured(NoiaSurfaceId sid);

#endif // NOIA_WAYLAND_STATE_H

