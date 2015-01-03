// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils-log.h"
#include "utils-chain.h"
#include "utils-store.h"
#include "event-timer.h"
#include "event-signals.h"
#include "surface-priv.h"

#include <malloc.h>
#include <memory.h>

static AuraStore* sStore = NULL;
static Chain* visible_surfaces = NULL; // TODO: move to compositor

//------------------------------------------------------------------------------

// TODO: move to compositor
void aura_surface_manager_redraw_all(AuraRenderer* renderer)
{
    if (!renderer) {
        LOG_ERROR("Invalid renderer!");
        return;
    }
    if (!renderer->draw) {
        LOG_ERROR("Wrong renderer implementation!");
        return;
    }

    renderer->draw((struct AuraRenderer*) renderer,
                   visible_surfaces);

    // TODO: pass as list
    if (visible_surfaces) {
        Link* link;
        for (link = visible_surfaces->first; link; link = link->next) {
            SurfaceId sid = (SurfaceId) link->data;
            aura_event_signal_emit(SIGNAL_SCREEN_REFRESH, (void*) sid);
        }
    }
}

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
    chain_append(visible_surfaces, (void*) sid);
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

void aura_surface_hide(SurfaceId sid)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface) {
        LOG_WARN2("Could not find surface (sid: %d)", sid);
        return;
    }

    surface->visible = 0;
}

//------------------------------------------------------------------------------

// TODO: Move to compositor
void on_display_found(void* data)
{
    AuraRenderer* renderer = (AuraRenderer*) data;
    if (!renderer) {
        LOG_ERROR("Invalid renderer");
        return;
    }

    LOG_INFO1("Adding new renderer!");

    // TODO: support for more renderers
    renderer->initialize((struct AuraRenderer*) renderer);
    renderer->data = aura_event_timer_run(100, // TODO: use displays frame rate
                             (AuraTimerHandler) aura_surface_manager_redraw_all,
                             data);
}

//------------------------------------------------------------------------------

void on_display_lost(void* data)
{
    AuraRenderer* renderer = (AuraRenderer*) data;
    if (!renderer) {
        LOG_ERROR("Invalid renderer");
        return;
    }

    LOG_INFO2("Deleting renderer timer");
    timer_t timerid = renderer->data;
    aura_event_timer_delete(timerid);
}

//------------------------------------------------------------------------------

// TBR
/*void aura_surface_manager_initialize(AuraLoop* this_loop)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    visible_surfaces = chain_new(NULL);

    aura_event_signal_subscribe(SIGNAL_DISPLAY_FOUND,
         aura_task_create(on_display_found, this_loop));

    aura_event_signal_subscribe(SIGNAL_DISPLAY_LOST,
         aura_task_create(on_display_lost, this_loop));
}*/

//------------------------------------------------------------------------------

