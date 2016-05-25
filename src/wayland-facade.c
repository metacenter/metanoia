// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "wayland-facade.h"

static NoiaWaylandContext* C;

//------------------------------------------------------------------------------

void noia_wayland_facade_initialize(NoiaWaylandContext* context)
{
    C = context;
}

//------------------------------------------------------------------------------

void noia_wayland_facade_finalize() {}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_wayland_facade_create_surface()
{
    return noia_surface_create(C->coordinator);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_commit(NoiaSurfaceId sid)
{
    noia_surface_commit(C->coordinator, sid);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_set_offset(NoiaSurfaceId sid, NoiaPosition pos)
{
    noia_surface_set_offset(C->coordinator, sid, pos);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_set_requested_size(NoiaSurfaceId sid, NoiaSize size)
{
    noia_surface_set_requested_size(C->coordinator, sid, size);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_set_requested_position(NoiaSurfaceId sid,
                                                NoiaSurfaceId reference_sid,
                                                NoiaPosition pos)
{
    noia_surface_set_requested_position(C->coordinator, sid, reference_sid, pos);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_set_cursor(int serial NOIA_UNUSED,
                                    int hotspot_x,
                                    int hotspot_y,
                                    NoiaSurfaceId sid)
{
    NoiaPosition hotspot = {hotspot_x, hotspot_y};
    noia_surface_set_offset(C->coordinator, sid, hotspot);
    noia_surface_set_as_cursor(sid);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_add_surface_resource
                                        (NoiaSurfaceId sid,
                                         NoiaWaylandSurfaceResourceType rc_type,
                                         struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);
    noia_wayland_cache_add_surface_resource(C->cache, sid, rc_type, rc);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_add_general_resource
                                        (NoiaWaylandGeneralResourceType rc_type,
                                         struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);
    noia_wayland_cache_add_general_resource(C->cache, rc_type, rc);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_remove_surface_resource
                                        (NoiaSurfaceId sid,
                                         NoiaWaylandSurfaceResourceType rc_type,
                                         struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);
    noia_wayland_cache_remove_surface_resource(C->cache, sid, rc_type, rc);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_remove_general_resource
                                        (NoiaWaylandGeneralResourceType rc_type,
                                         struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);
    noia_wayland_cache_remove_general_resource(C->cache, rc_type, rc);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_add_surface(NoiaSurfaceId sid, struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);
    noia_wayland_cache_create_surface(C->cache, sid);
    noia_wayland_cache_add_surface_resource
                       (C->cache, sid, NOIA_RESOURCE_SURFACE, rc);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_add_shell_surface(NoiaSurfaceId sid,
                                           NoiaWaylandSurfaceResourceType type,
                                           struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);
    noia_wayland_cache_add_surface_resource(C->cache, sid, type, rc);
    noia_surface_show(C->coordinator, sid, NOIA_SURFACE_SHOW_IN_SHELL);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_surface_attach(NoiaSurfaceId sid,
                                        struct wl_resource* rc,
                                        struct wl_resource* brc,
                                        int width,
                                        int height,
                                        int stride,
                                        uint8_t* data)
{
    noia_wayland_cache_lock(C->cache);
    noia_wayland_cache_add_surface_resource
                       (C->cache, sid, NOIA_RESOURCE_BUFFER, brc);
    noia_surface_attach(C->coordinator, sid, width, height, stride, data, rc);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_remove_surface(NoiaSurfaceId sid,
                                        struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);
    noia_surface_destroy(C->coordinator, sid);
    noia_wayland_cache_remove_surface_resource
                       (C->cache, sid, NOIA_RESOURCE_SURFACE, rc);
    noia_wayland_cache_remove_surface(C->cache, sid);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

NoiaItemId noia_wayland_facade_create_region()
{
    noia_wayland_cache_lock(C->cache);
    NoiaItemId result = noia_wayland_cache_create_region(C->cache);
    noia_wayland_cache_unlock(C->cache);
    return result;
}

//------------------------------------------------------------------------------

void noia_wayland_facade_inflate_region(NoiaItemId rid,
                                        int x, int y,
                                        int width, int height)
{
    noia_wayland_cache_lock(C->cache);
    NoiaWaylandRegion* region = noia_wayland_cache_find_region(C->cache, rid);
    noia_wayland_region_inflate(region, x, y, width, height);
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_set_input_region(NoiaSurfaceId sid, NoiaItemId rid)
{
    noia_wayland_cache_lock(C->cache);
    NoiaWaylandRegion* region = noia_wayland_cache_find_region(C->cache, rid);
    if (region) {
        noia_surface_set_offset(C->coordinator, sid, region->pos);
        noia_surface_set_requested_size(C->coordinator, sid, region->size);
    }
    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

void noia_wayland_facade_add_keyboard_resource(struct wl_resource* rc)
{
    noia_wayland_cache_lock(C->cache);

    // Store new resource
    noia_wayland_cache_add_general_resource
                       (C->cache, NOIA_RESOURCE_KEYBOARD, rc);

    // If client is focused, send enter event
    NoiaWaylandRc focused =
              noia_wayland_cache_get_rc_for_sid(C->cache,
                                                C->state->keyboard_focused_sid);

    struct wl_client* rc_client = wl_resource_get_client(rc);
    if (focused.cl and rc_client and (rc_client == focused.cl)) {
        struct wl_array array;
        wl_array_init(&array);
        int serial = noia_wayland_engine_next_serial(C->engine);
        wl_keyboard_send_enter(rc, serial, focused.rc, &array);
    }

    noia_wayland_cache_unlock(C->cache);
}

//------------------------------------------------------------------------------

