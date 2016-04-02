// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_WAYLAND_CACHE_H
#define NOIA_WAYLAND_CACHE_H

#include "utils-store.h"
#include "wayland-region.h"
#include "wayland-surface.h"
#include "utils-list.h"

#include <wayland-server.h>

//------------------------------------------------------------------------------

NoiaResult noia_wayland_cache_initialize(void);

void noia_wayland_cache_finalize(void);

//------------------------------------------------------------------------------

void noia_wayland_cache_add_surface(NoiaSurfaceId sid,
                                    NoiaWaylandSurface* surface);

NoiaWaylandSurface* noia_wayland_cache_find_surface(NoiaSurfaceId sid);

void noia_wayland_cache_remove_surface(NoiaSurfaceId sid);

//------------------------------------------------------------------------------

NoiaItemId noia_wayland_cache_create_region(void);

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
                                  (NoiaSurfaceId sid,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

void noia_wayland_cache_remove_general_resource
                                  (NoiaWaylandGeneralResourceType resource_type,
                                   struct wl_resource* resource);

NoiaList* noia_wayland_cache_get_resources
                                 (NoiaWaylandGeneralResourceType resource_type);

//------------------------------------------------------------------------------

#endif // NOIA_WAYLAND_CACHE_H

