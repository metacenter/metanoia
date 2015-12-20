// file: wayland-surface.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_SURFACE_H
#define NOIA_WAYLAND_SURFACE_H

#include "global-types.h"
#include "wayland-types.h"
#include "utils-list.h"

#include <wayland-server.h>

/// Structure used by cache for storing surface-refated resources.
/// @note Qt applications use two frames.
typedef struct {
    NoiaItem base;
    NoiaList* frame_resources;
    struct wl_resource* resources[NOIA_NUM_SURFACE_RESOURCE_TYPES];
} NoiaWaylandSurface;

/// Wayland surface constructor.
NoiaWaylandSurface* noia_wayland_surface_new(void);

/// Wayland surface destructor.
void noia_wayland_surface_free(NoiaWaylandSurface* self);

/// Get resource of given type.
struct wl_resource* noia_wayland_surface_get_resource
                                 (NoiaWaylandSurface* self,
                                  NoiaWaylandSurfaceResourceType resource_type);

/// Get list of frame resources.
const NoiaList* noia_wayland_surface_get_frame_resources
                                                     (NoiaWaylandSurface* self);

/// Add resource of given type.
void noia_wayland_surface_add_resource
                                  (NoiaWaylandSurface* self,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

/// Remove resource of given type.
void noia_wayland_surface_remove_resource
                                  (NoiaWaylandSurface* self,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

/// Remove all resources.
void noia_wayland_surface_remove_frame_resources(NoiaWaylandSurface* self);

#endif // NOIA_WAYLAND_SURFACE_H

