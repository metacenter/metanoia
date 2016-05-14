// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_WAYLAND_STATE_H
#define NOIA_WAYLAND_STATE_H

#include "utils-store.h"
#include "output.h"
#include "wayland-surface.h"

#include <wayland-server.h>

NoiaResult noia_wayland_state_initialize(struct wl_display* display,
                                         NoiaCoordinator* coordinator);

void noia_wayland_state_finalize(void);

NoiaSurfaceId noia_wayland_state_create_surface();

void noia_wayland_state_add_surface(NoiaItemId sid, struct wl_resource* rc);

void noia_wayland_state_add_shell_surface(NoiaSurfaceId sid,
                                          NoiaWaylandSurfaceResourceType type,
                                          struct wl_resource* rc);

void noia_wayland_state_remove_surface(NoiaItemId sid, struct wl_resource* rc);

void noia_wayland_state_surface_attach(NoiaSurfaceId sid,
                                       struct wl_resource* rc,
                                       struct wl_resource* brc,
                                       int width,
                                       int height,
                                       int stride,
                                       uint8_t* buffer);

void noia_wayland_state_surface_commit(NoiaItemId sid);

void noia_wayland_state_surface_set_offset(NoiaItemId sid, NoiaPosition pos);

void noia_wayland_state_surface_set_requested_size(NoiaItemId sid,
                                                   NoiaSize size);

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

