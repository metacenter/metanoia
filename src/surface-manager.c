// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils-log.h"
#include "event-timer.h"
#include "event-signals.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>
#include <pthread.h>

static NoiaStore* sStore = NULL;
pthread_mutex_t surface_mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

#define NOIA_GET_AND_ASSERT_SURFACE(NAME, SID) \
    NoiaSurfaceData* NAME = noia_surface_get(SID); \
    if (!NAME) {LOG_WARN2("Could not find surface (sid: %d)", SID); return;}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_surface_create(void)
{
    if (!sStore) {
        sStore = noia_store_new_for_id();
    }

    // Create new surface
    NoiaSurfaceData* data = noia_surface_data_new();
    if (!data) {
        LOG_ERROR("Could not create new surface!");
        return scInvalidSurfaceId;
    }

    NoiaItemId sid = noia_store_generate_new_id(sStore);
    noia_store_add(sStore, sid, data);

    return sid;
}

//------------------------------------------------------------------------------

void noia_surface_destroy(NoiaSurfaceId sid)
{
    noia_event_signal_emit_int(SIGNAL_SURFACE_DESTROYED, sid);
    noia_surface_lock();
    noia_surface_data_free(noia_store_delete(sStore, sid));
    noia_surface_unlock();
}

//------------------------------------------------------------------------------

NoiaSurfaceData* noia_surface_get(NoiaSurfaceId sid)
{
    return noia_store_find(sStore, sid);
}

//------------------------------------------------------------------------------

void noia_surface_clear_all(void)
{
    noia_surface_lock();
    noia_store_free_with_items(sStore, (NoiaFreeFunc) noia_surface_data_free);
    noia_surface_unlock();
}

//------------------------------------------------------------------------------

void noia_surface_attach_egl(NoiaSurfaceId sid,
                             void* resource)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    surface->buffer.resource = resource;
    /// @todo Log at init if renderer supports egl
    /// @todo Move 'attach' out from renderer
    //if (renderer && renderer->attach) {
    //    renderer->attach((renderer, sid, resource);
    //}
}

//------------------------------------------------------------------------------

void noia_surface_attach(NoiaSurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* data,
                         void* resource)
{
    NOIA_ENSURE(data or resource, return);
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    noia_surface_lock();

    surface->pending_buffer.width    = width;
    surface->pending_buffer.height   = height;
    surface->pending_buffer.stride   = stride;
    surface->pending_buffer.data     = data;
    surface->pending_buffer.resource = resource;

    noia_surface_unlock();
}

//------------------------------------------------------------------------------

void noia_surface_commit(NoiaSurfaceId sid)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    noia_surface_lock();

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
        /// @todo Size should not be set here.
        if ((surface->requested_size.width  == 0)
        or  (surface->requested_size.height == 0)) {
            surface->requested_size.width  = surface->buffer.width;
            surface->requested_size.height = surface->buffer.height;
        }
        noia_event_signal_emit_int(SIGNAL_SURFACE_READY, sid);
    }

    noia_surface_unlock();
}

//------------------------------------------------------------------------------

void noia_surface_show(NoiaSurfaceId sid NOIA_UNUSED)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    surface->is_toplevel = true;
}

//------------------------------------------------------------------------------

void noia_surface_set_offset(NoiaSurfaceId sid, NoiaPosition offset)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    surface->offset = offset;
}

//------------------------------------------------------------------------------

void noia_surface_set_desired_size(NoiaSurfaceId sid, NoiaSize size)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);

    if (size.width < 0 || size.height < 0) {
        size = surface->requested_size;
    }

    if (surface->desired_size.width != size.width
    ||  surface->desired_size.height != size.height) {
        surface->desired_size = size;
        noia_event_signal_emit_int(SIGNAL_SURFACE_RECONFIGURED, sid);
    }
}

//------------------------------------------------------------------------------

void noia_surface_set_requested_size(NoiaSurfaceId sid, NoiaSize size)
{
    NOIA_GET_AND_ASSERT_SURFACE(surface, sid);
    surface->requested_size = size;
}

//------------------------------------------------------------------------------

void noia_surface_set_as_cursor(NoiaSurfaceId sid)
{
    noia_event_signal_emit_int(SIGNAL_CURSOR_SURFACE_CHANGE, sid);
}

//------------------------------------------------------------------------------

void noia_surface_lock(void)
{
    pthread_mutex_lock(&surface_mutex);
}

//------------------------------------------------------------------------------

void noia_surface_unlock(void)
{
    pthread_mutex_unlock(&surface_mutex);
}

//------------------------------------------------------------------------------

int noia_surface_compare(NoiaSurfaceId first, NoiaSurfaceId second)
{
    if (first < second) return -1;
    if (first > second) return  1;
    return 0;
}

//------------------------------------------------------------------------------

