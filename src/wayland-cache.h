// file: wayland-cache.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_WAYLAND_CACHE_H__
#define __NOIA_WAYLAND_CACHE_H__

#include "utils-store.h"
#include "wayland-region.h"
#include "wayland-surface.h"
#include "utils-list.h"

#include <wayland-server.h>

//------------------------------------------------------------------------------

NoiaResult noia_wayland_cache_initialize();

void noia_wayland_cache_finalize();

//------------------------------------------------------------------------------

void noia_wayland_cache_add_surface(NoiaSurfaceId sid,
                                    NoiaWaylandSurface* surface);

NoiaWaylandSurface* noia_wayland_cache_find_surface(NoiaSurfaceId sid);

void noia_wayland_cache_remove_surface(NoiaSurfaceId sid);

//------------------------------------------------------------------------------

NoiaItemId noia_wayland_cache_create_region();

NoiaWaylandRegion* noia_wayland_cache_find_region(NoiaItemId rid);

//------------------------------------------------------------------------------

void noia_wayland_cache_add_surface_resource
                                  (NoiaSurfaceId sid,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

void noia_wayland_cache_add_general_resource
                                  (NoiaWaylandGeneralResourceType resource_type,
                                   struct wl_resource* resource);

void noia_wayland_cache_remove_surface_resource
                                  (NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

void noia_wayland_cache_remove_general_resource
                                  (NoiaWaylandGeneralResourceType resource_type,
                                   struct wl_resource* resource);

NoiaList* noia_wayland_cache_get_resources
                                 (NoiaWaylandGeneralResourceType resource_type);

//------------------------------------------------------------------------------

#endif // __NOIA_WAYLAND_CACHE_H__

