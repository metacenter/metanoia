// file: wayland-surface.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_SURFACE_H
#define NOIA_WAYLAND_SURFACE_H

#include "global-types.h"
#include "wayland-types.h"

#include <wayland-server.h>

typedef struct {
    NoiaItem base;
    struct wl_resource* resources[NOIA_NUM_SURFACE_RESOURCE_TYPES];
} NoiaWaylandSurface;

NoiaWaylandSurface* noia_wayland_surface_new(void);

void noia_wayland_surface_free(NoiaWaylandSurface* self);

struct wl_resource* noia_wayland_surface_get_resource
                                 (NoiaWaylandSurface* self,
                                  NoiaWaylandSurfaceResourceType resource_type);

void noia_wayland_surface_set_resource
                                  (NoiaWaylandSurface* self,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

#endif // NOIA_WAYLAND_SURFACE_H

