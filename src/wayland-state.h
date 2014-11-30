// file: wayland-state.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_STATE_H__
#define __AURA_WAYLAND_STATE_H__

#include "utils-store.h"

#include <wayland-server.h>

typedef struct AuraSurfaceWaylandData {
    AuraItem base;
    struct wl_resource* resource;
    struct wl_resource* buffer_resource;
    struct wl_resource* frame_resource;
} AuraSurfaceWaylandData;

AuraSurfaceWaylandData* wayland_surface_data_new(void);

int wayland_state_initialize();

void wayland_state_finalize();

void wayland_state_add_surface(AuraItemId sid, struct wl_resource* rc);

void wayland_state_surface_attach(AuraItemId sid, struct wl_resource* rc);

void wayland_state_subscribe_frame(AuraItemId sid, struct wl_resource* rc);

//void wayland_state_notify_frame(AuraItemId sid);

void wayland_state_add_keyboard_resource(struct wl_resource* keyboard_rc);

void wayland_state_keyboard_focus_update(AuraItemId sid);

void wayland_state_screen_refresh();

#endif // __AURA_WAYLAND_STATE_H__

