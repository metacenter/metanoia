// file: wayland-surface.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_SURFACE_H__
#define __AURA_WAYLAND_SURFACE_H__

#include "global-types.h"
#include "wayland-types.h"

#include <wayland-server.h>

typedef struct {
    AuraItem base;
    struct wl_resource* resources[AURA_NUM_SURFACE_RESOURCE_TYPES];
} AuraWaylandSurface;

AuraWaylandSurface* aura_wayland_surface_new(void);

void aura_wayland_surface_free(AuraWaylandSurface* self);

struct wl_resource* aura_wayland_surface_get_resource
                                 (AuraWaylandSurface* self,
                                  AuraWaylandSurfaceResourceType resource_type);

void aura_wayland_surface_set_resource
                                  (AuraWaylandSurface* self,
                                   AuraWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

#endif // __AURA_WAYLAND_SURFACE_H__

