// file: wayland-surface.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_SURFACE_H__
#define __AURA_WAYLAND_SURFACE_H__

#include "global-types.h"
#include <wayland-server.h>

typedef struct {
    AuraItem base;
    struct wl_resource* resource;
    struct wl_resource* buffer_resource;
    struct wl_resource* frame_resource;
    struct wl_resource* shell_resource;
    struct wl_resource* xdg_shell_resource;
} AuraWaylandSurface;

AuraWaylandSurface* aura_wayland_surface_new(void);

void aura_wayland_surface_free(AuraWaylandSurface* self);

#endif // __AURA_WAYLAND_SURFACE_H__

