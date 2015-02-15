// file: wayland-state.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_STATE_H__
#define __AURA_WAYLAND_STATE_H__

#include "utils-store.h"
#include "output.h"
#include "wayland-surface.h"

#include <wayland-server.h>

AuraResult aura_wayland_state_initialize(struct wl_display* display);

void aura_wayland_state_finalize();

void aura_wayland_state_add_surface(AuraItemId sid, struct wl_resource* rc);

void aura_wayland_state_remove_surface(AuraItemId sid);

void aura_wayland_state_surface_attach(AuraItemId sid, struct wl_resource* rc);

void aura_wayland_state_subscribe_frame(AuraItemId sid, struct wl_resource* rc);

void aura_wayland_state_add_keyboard_resource(struct wl_resource* rc);

void aura_wayland_state_keyboard_focus_update(AuraItemId sid);

void aura_wayland_state_key(uint32_t time, uint32_t key, uint32_t state);

void aura_wayland_state_pointer_focus_update(AuraSurfaceId new_sid,
                                             AuraPosition pos);

void aura_wayland_state_pointer_motion(AuraSurfaceId sid, AuraPosition pos);

void aura_wayland_state_pointer_button(uint32_t time,
                                       uint32_t button,
                                       uint32_t state);

void aura_wayland_state_set_cursor(int serial,
                                   int hotspot_x,
                                   int hotspot_y,
                                   AuraSurfaceId sid);

void aura_wayland_state_screen_refresh();

void aura_wayland_state_advertise_output(AuraOutput* output);

void aura_wayland_state_destroy_output(AuraOutput* output);

void aura_wayland_state_surface_reconfigured(AuraSurfaceId sid);

#endif // __AURA_WAYLAND_STATE_H__

