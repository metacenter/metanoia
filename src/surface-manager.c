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

    aura_event_signal_emit(SIGNAL_SURFACE_CREATED, (void*) sid);

    // TODO: Do this as strategy
    aura_event_signal_emit(SIGNAL_KEYBOARD_FOCUS_CHANGED, (void*) sid);

    return sid;
}

//------------------------------------------------------------------------------

void aura_surface_destroy(SurfaceId sid)
{
    aura_store_delete(sStore, sid);
}

//------------------------------------------------------------------------------

AuraSurfaceData* aura_surface_get(SurfaceId sid)
{
    return aura_store_get(sStore, sid);
}

//------------------------------------------------------------------------------

void aura_surface_attach_egl(SurfaceId sid,
                             void* resource)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface) {
        LOG_WARN2("Could not find surface (sid: %d)", sid);
        return;
    }

    // TODO: log at init if renderer supports egl
    // TODO: move 'attach' out from renderer
    //if (renderer && renderer->attach) {
    //    renderer->attach((struct AuraRenderer*) renderer, sid, resource);
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

    surface->buffer.width  = width;
    surface->buffer.height = height;
    surface->buffer.stride = stride;
    surface->buffer.data   = data;
    surface->visible = 1;
}

//------------------------------------------------------------------------------

