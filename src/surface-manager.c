// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils-log.h"
#include "event-timer.h"
#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

static AuraStore* sStore = NULL;

//------------------------------------------------------------------------------

#define AURA_GET_AND_ASSERT_SURFACE(NAME, SID) \
    AuraSurfaceData* NAME = aura_surface_get(SID); \
    if (!NAME) {LOG_WARN2("Could not find surface (sid: %d)", SID); return;}

//------------------------------------------------------------------------------

AuraSurfaceId aura_surface_create(void)
{
    if (!sStore) {
        sStore = aura_store_new_for_id();
    }

    // Create new surface
    AuraSurfaceData* data = aura_surface_data_new();
    if (!data) {
        LOG_ERROR("Could not create new surface!");
        return scInvalidSurfaceId;
    }

    AuraItemId sid = aura_store_generate_new_id(sStore);
    aura_store_add(sStore, sid, data);

    return sid;
}

//------------------------------------------------------------------------------

void aura_surface_destroy(AuraSurfaceId sid)
{
    aura_event_signal_emit_int(SIGNAL_SURFACE_DESTROYED, sid);
    aura_store_delete(sStore, sid);
}

//------------------------------------------------------------------------------

AuraSurfaceData* aura_surface_get(AuraSurfaceId sid)
{
    return aura_store_find(sStore, sid);
}

//------------------------------------------------------------------------------

void aura_surface_clear_all()
{
    aura_store_free_with_items(sStore, (AuraFreeFunc) aura_surface_data_free);
}

//------------------------------------------------------------------------------

void aura_surface_attach_egl(AuraSurfaceId sid,
                             AURA_UNUSED void* resource)
{
    AURA_GET_AND_ASSERT_SURFACE(surface, sid);

    // TODO: log at init if renderer supports egl
    // TODO: move 'attach' out from renderer
    //if (renderer && renderer->attach) {
    //    renderer->attach((renderer, sid, resource);
    //}
}

//------------------------------------------------------------------------------

void aura_surface_commit(AuraSurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* data)
{
    AURA_GET_AND_ASSERT_SURFACE(surface, sid);

    int is_first_time_commited = !surface->buffer.data;

    surface->buffer.width  = width;
    surface->buffer.height = height;
    surface->buffer.stride = stride;
    surface->buffer.data   = data;

    if (is_first_time_commited) {
        if (surface->requested_size.width  == 0
        ||  surface->requested_size.height == 0) {
            surface->requested_size.width  = width;
            surface->requested_size.height = height;
        }
        aura_event_signal_emit_int(SIGNAL_SURFACE_CREATED, sid);
    }
}

//------------------------------------------------------------------------------

void aura_surface_set_requested_size(AuraSurfaceId sid,
                                     AuraSize size)
{
    AURA_GET_AND_ASSERT_SURFACE(surface, sid);
    surface->requested_size = size;
}

//------------------------------------------------------------------------------

int aura_surface_compare(AuraSurfaceId first, AuraSurfaceId second)
{
    if (first < second) return -1;
    if (first > second) return  1;
    return 0;
}

//------------------------------------------------------------------------------

