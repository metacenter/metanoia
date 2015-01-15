// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils-log.h"
#include "utils-chain.h"
#include "utils-store.h"
#include "event-timer.h"
#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

static AuraStore* sStore = NULL;

//------------------------------------------------------------------------------

SurfaceId aura_surface_create(void)
{
    if (!sStore) {
        sStore = aura_store_new();
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

void aura_surface_destroy(SurfaceId sid)
{
    aura_event_signal_emit(SIGNAL_SURFACE_DESTROYED, (void*) sid);
    aura_store_delete(sStore, sid);
}

//------------------------------------------------------------------------------

AuraSurfaceData* aura_surface_get(SurfaceId sid)
{
    return aura_store_get(sStore, sid);
}

//------------------------------------------------------------------------------

void aura_surface_attach_egl(SurfaceId sid,
                             AURA_UNUSED void* resource)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface) {
        LOG_WARN2("Could not find surface (sid: %d)", sid);
        return;
    }

    // TODO: log at init if renderer supports egl
    // TODO: move 'attach' out from renderer
    //if (renderer && renderer->attach) {
    //    renderer->attach((renderer, sid, resource);
    //}
}

//------------------------------------------------------------------------------

void aura_surface_commit(SurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* data)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface) {
        LOG_WARN2("Could not find surface (sid: %d)", sid);
        return;
    }

    int is_first_time_commited = !surface->buffer.data;

    surface->buffer.width  = width;
    surface->buffer.height = height;
    surface->buffer.stride = stride;
    surface->buffer.data   = data;

    if (is_first_time_commited) {
        aura_event_signal_emit(SIGNAL_SURFACE_CREATED, (void*) sid);
    }
}

//------------------------------------------------------------------------------

int aura_surface_compare(SurfaceId first, SurfaceId second)
{
    if (first < second) return -1;
    if (first > second) return  1;
    return 0;
}

//------------------------------------------------------------------------------

