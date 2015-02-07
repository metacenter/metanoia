// file: wayland-cache.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_CACHE_H__
#define __AURA_WAYLAND_CACHE_H__

#include "utils-store.h"
#include "wayland-surface.h"
#include "utils-chain.h"

#include <wayland-server.h>

AuraResult aura_wayland_cache_initialize();

void aura_wayland_cache_finalize();

void aura_wayland_cache_add_surface(AuraSurfaceId sid,
                                    AuraWaylandSurface* surface);

AuraWaylandSurface* aura_wayland_cache_find_surface(AuraSurfaceId sid);

void aura_wayland_cache_remove_surface(AuraSurfaceId sid);

void aura_wayland_cache_add_surface_resource
                                  (AuraSurfaceId sid,
                                   AuraWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

void aura_wayland_cache_add_general_resource
                                  (AuraWaylandGeneralResourceType resource_type,
                                   struct wl_resource* resource);

void aura_wayland_cache_remove_surface_resource
                                  (AuraWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource);

void aura_wayland_cache_remove_general_resource
                                  (AuraWaylandGeneralResourceType resource_type,
                                   struct wl_resource* resource);

Chain* aura_wayland_cache_get_resources
                                 (AuraWaylandGeneralResourceType resource_type);

#endif // __AURA_WAYLAND_CACHE_H__

