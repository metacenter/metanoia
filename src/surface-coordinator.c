// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "surface-coordinator.h"

#include "utils-log.h"
#include "utils-time.h"
#include "event-signals.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>
#include <pthread.h>

//------------------------------------------------------------------------------

#define NOIA_GET_AND_ASSERT_SURFACE(NAME, SID) \
    NoiaSurfaceData* NAME = noia_surface_get(coordinator, SID); \
    if (!NAME) {LOG_WARN2("Could not find surface (sid: %d)", SID); return;}

//------------------------------------------------------------------------------
// COORDINATOR

struct NoiaCoordinatorStruct {
    NoiaStore* surfaces;
    NoiaMilliseconds last_notify_time;
    NoiaSurfaceId kfsid;
    pthread_mutex_t surface_mutex;
};

//------------------------------------------------------------------------------

NoiaCoordinator* noia_coordinator_new()
{
    NoiaCoordinator* self = malloc(sizeof(*self));

    self->surfaces = noia_store_new_for_id();
    self->last_notify_time = 0;
    self->kfsid = scInvalidSurfaceId;
    pthread_mutex_init(&self->surface_mutex, NULL);

    return self;
}

//------------------------------------------------------------------------------

void noia_coordinator_free(NoiaCoordinator* self)
{
    NOIA_ENSURE(self, return);

    noia_coordinator_lock_surfaces(self);
    noia_store_free_with_items
                        (self->surfaces, (NoiaFreeFunc) noia_surface_data_free);
    noia_coordinator_unlock_surfaces(self);
    free(self);
}

//------------------------------------------------------------------------------

void noia_coordinator_lock_surfaces(NoiaCoordinator* self)
{
    pthread_mutex_lock(&self->surface_mutex);
}

//------------------------------------------------------------------------------

void noia_coordinator_unlock_surfaces(NoiaCoordinator* self)
{
    pthread_mutex_unlock(&self->surface_mutex);
}

//------------------------------------------------------------------------------

void noia_coordinator_notify(NoiaCoordinator* self)
{
    self->last_notify_time = noia_time_get_monotonic_milliseconds();
}

//------------------------------------------------------------------------------

NoiaMilliseconds noia_coordinator_get_last_notify(NoiaCoordinator* self)
{
    return self->last_notify_time;
}

//------------------------------------------------------------------------------
// SURFACES

NoiaSurfaceId noia_surface_create(NoiaCoordinator* coordinator)
{
    // Create new surface
    NoiaSurfaceData* data = noia_surface_data_new();
    NOIA_ENSURE(data, return scInvalidSurfaceId);

    noia_coordinator_lock_surfaces(coordinator);
    NoiaSurfaceId sid = noia_store_generate_new_id(coordinator->surfaces);
    noia_store_add(coordinator->surfaces, sid, data);
    noia_list_append(data->satellites, (void*) sid);
    noia_coordinator_unlock_surfaces(coordinator);

    return sid;
}

//------------------------------------------------------------------------------

void noia_surface_destroy(NoiaCoordinator* coordinator, NoiaSurfaceId sid)
{
    noia_event_signal_emit_int(SIGNAL_SURFACE_DESTROYED, sid);
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    NOIA_GET_AND_ASSERT_SURFACE(parent_surface, surface->parent_sid);

    noia_coordinator_lock_surfaces(coordinator);
    noia_list_remove_all(parent_surface->satellites, (void*) sid,
                         (NoiaCompareFunc) noia_surface_compare);
    noia_surface_data_free(noia_store_delete(coordinator->surfaces, sid));
    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

NoiaSurfaceData* noia_surface_get(NoiaCoordinator* coordinator,
                                  NoiaSurfaceId sid)
{
    return noia_store_find(coordinator->surfaces, sid);
}

//------------------------------------------------------------------------------

void noia_surface_attach_egl(NoiaCoordinator* coordinator,
                             NoiaSurfaceId sid,
                             void* resource)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    surface->buffer.resource = resource;
}

//------------------------------------------------------------------------------

void noia_surface_attach(NoiaCoordinator* coordinator,
                         NoiaSurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* data,
                         void* resource)
{
    NOIA_ENSURE(data or resource, return);
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    noia_coordinator_lock_surfaces(coordinator);

    surface->pending_buffer.width    = width;
    surface->pending_buffer.height   = height;
    surface->pending_buffer.stride   = stride;
    surface->pending_buffer.data     = data;
    surface->pending_buffer.resource = resource;

    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

void noia_surface_commit(NoiaCoordinator* coordinator, NoiaSurfaceId sid)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    noia_coordinator_lock_surfaces(coordinator);

    int is_first_time_commited = (not surface->buffer.data)
                             and (not surface->buffer.resource);

    // Copy buffer
    if (surface->pending_buffer.data) {
        size_t size = surface->pending_buffer.stride
                    * surface->pending_buffer.height;
        size_t old_size = surface->buffer.stride * surface->buffer.height;

        if (size != old_size) {
            if (surface->buffer.data) {
                free(surface->buffer.data);
            }
            surface->buffer.data = malloc(size);
        }
        memcpy(surface->buffer.data, surface->pending_buffer.data, size);
        surface->buffer.resource = NULL;
    } else if (surface->pending_buffer.resource) {
        surface->buffer.data = NULL;
        surface->buffer.resource = surface->pending_buffer.resource;
    }

    surface->buffer.width  = surface->pending_buffer.width;
    surface->buffer.height = surface->pending_buffer.height;
    surface->buffer.stride = surface->pending_buffer.stride;

    // If surface was just created - inform others
    if (is_first_time_commited) {
        if ((surface->requested_size.width  == 0)
        or  (surface->requested_size.height == 0)) {
            surface->requested_size.width  = surface->buffer.width;
            surface->requested_size.height = surface->buffer.height;
            if (surface->parent_sid != scInvalidSurfaceId) {
                surface->desired_size.width  = surface->buffer.width;
                surface->desired_size.height = surface->buffer.height;
            }
        }

        if (surface->buffer.data or surface->buffer.resource) {
            noia_surface_show(coordinator, sid, NOIA_SURFACE_SHOW_DRAWABLE);
        }
    }

    noia_coordinator_notify(coordinator);

    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

void noia_surface_show(NoiaCoordinator* coordinator,
                       NoiaSurfaceId sid,
                       NoiaSurfaceShowReason reason)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);

    surface->show_flags |= reason;
    if (surface->show_flags == NOIA_SURFACE_SHOW_FULL) {
        noia_event_signal_emit_int(SIGNAL_SURFACE_READY, sid);
    }
}

//------------------------------------------------------------------------------

void noia_surface_reconfigure(NoiaCoordinator* coordinator,
                              NoiaSurfaceId sid,
                              NoiaSize size,
                              uint8_t state_flags)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);

    if ((size.width < 0) or (size.height < 0)) {
        size = surface->requested_size;
    }

    if ((surface->desired_size.width != size.width)
    or  (surface->desired_size.height != size.height)
    or  (surface->state_flags != state_flags)) {
        surface->desired_size = size;
        surface->state_flags = state_flags;
        noia_event_signal_emit_int(SIGNAL_SURFACE_RECONFIGURED, sid);
    }
}

//------------------------------------------------------------------------------

void noia_surface_set_focus(NoiaCoordinator* coordinator, NoiaSurfaceId new_sid)
{
    noia_coordinator_lock_surfaces(coordinator);

    NoiaSurfaceId old_sid = coordinator->kfsid;
    if (old_sid != new_sid) {
        coordinator->kfsid = new_sid;
        noia_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED, new_sid);
    }

    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

void noia_surface_set_offset(NoiaCoordinator* coordinator,
                             NoiaSurfaceId sid,
                             NoiaPosition offset)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    noia_coordinator_lock_surfaces(coordinator);
    surface->offset.x = (offset.x > 0) ? offset.x : 0;
    surface->offset.y = (offset.y > 0) ? offset.y : 0;
    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

void noia_surface_set_requested_size(NoiaCoordinator* coordinator,
                                     NoiaSurfaceId sid,
                                     NoiaSize size)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    noia_coordinator_lock_surfaces(coordinator);
    surface->requested_size = size;
    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

void noia_surface_set_relative_position(NoiaCoordinator* coordinator,
                                        NoiaSurfaceId sid,
                                        NoiaPosition pos)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    noia_coordinator_lock_surfaces(coordinator);
    surface->relative_position = pos;
    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

void noia_surface_relate(NoiaCoordinator* coordinator,
                         NoiaSurfaceId sid,
                         NoiaSurfaceId parent_sid)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    NOIA_GET_AND_ASSERT_SURFACE(parent_surface, parent_sid);
    noia_coordinator_lock_surfaces(coordinator);
    surface->parent_sid = parent_sid;
    surface->relative_position.x = 0;
    surface->relative_position.y = 0;
    noia_list_append(parent_surface->satellites, (void*) sid);
    noia_coordinator_unlock_surfaces(coordinator);
}

//------------------------------------------------------------------------------

void noia_surface_to_array(NoiaCoordinator* coordinator,
                           NoiaSurfaceId sid,
                           NoiaPosition parent_pos,
                           NoiaPool* surfaces)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);

    FOR_EACH(surface->satellites, link) {
        NoiaSurfaceId child_sid = (NoiaSurfaceId) link->data;
        if (child_sid == sid) {
            NoiaSurfaceContext* context = noia_pool_add(surfaces);
            context->sid = sid;
            context->pos.x = parent_pos.x + surface->relative_position.x;
            context->pos.y = parent_pos.y + surface->relative_position.y;
        } else {
            NoiaSurfaceData* data = noia_surface_get(coordinator, child_sid);
            if (data) {
                noia_surface_to_array
                                 (coordinator, child_sid, parent_pos, surfaces);
            }
        }
    }
}

//------------------------------------------------------------------------------
// HELPERS

void noia_surface_set_as_cursor(NoiaSurfaceId sid)
{
    noia_event_signal_emit_int(SIGNAL_CURSOR_SURFACE_CHANGE, sid);
}

//------------------------------------------------------------------------------

int noia_surface_compare(NoiaSurfaceId first, NoiaSurfaceId second)
{
    if (first < second) return -1;
    if (first > second) return  1;
    return 0;
}

//------------------------------------------------------------------------------

