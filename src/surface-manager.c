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
static AuraRenderer* renderer = NULL; // TODO: move to compositor

//------------------------------------------------------------------------------

// TODO: move to compositor
void aura_surface_manager_redraw_all()
{
    if (renderer == NULL) {
        LOG_ERROR("Invalid renderer!");
        return;
    }
    if (renderer->draw == NULL) {
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
    // Create new surface
    AuraSurfaceData* data = aura_surface_data_new();
    if (data == NULL) {
        LOG_ERROR("Could not create new surface!");
        return scInvalidSurfaceId;
    }

    AuraItemId sid = aura_store_generate_new_id(sStore);
    aura_store_add(sStore, sid, data);

    // Do this as strategy
    chain_append(visible_surfaces, (void*) sid);
    aura_event_signal_emit(SIGNAL_KEYBOARD_FOCUS_CHANGED, (void*) sid);

    return sid;
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
    if (surface == NULL) {
        LOG_WARN2("Could not find surface (sid: %d)", sid);
        return;
    }

    // TODO: log at init if renderer supports egl
    if (renderer && renderer->attach) {
        renderer->attach((struct AuraRenderer*) renderer, sid, resource);
    }
}

//------------------------------------------------------------------------------

void aura_surface_commit(SurfaceId sid,
                         int width,
                         int height,
                         int stride,
                         uint8_t* data)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (surface == 0) {
        LOG_WARN2("Could not find surface (sid: %d)", sid);
        return;
    }

    surface->buffer.width  = width;
    surface->buffer.height = height;
    surface->buffer.stride = stride;
    surface->buffer.data   = data;
}

//------------------------------------------------------------------------------

// TODO: Move to compositor
static void on_display_found(void* data)
{
    AuraRenderer* renderer_new = (AuraRenderer*) data;
    if (renderer_new == NULL) {
        LOG_ERROR("New renderer is invalid!");
        return;
    }

    LOG_INFO1("Adding new renderer!");

    // TODO: support for more renderers
    renderer = renderer_new;
    renderer->initialize((struct AuraRenderer*) renderer);
    aura_event_timer_run(aura_surface_manager_redraw_all, 100);
}

//------------------------------------------------------------------------------

void aura_surface_manager_initialize(AuraLoop* this_loop)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    visible_surfaces = chain_new(NULL);
    sStore = aura_store_new();

    aura_event_signal_subscribe(SIGNAL_DISPLAY_FOUND,
         aura_task_create(on_display_found, this_loop));
}

//------------------------------------------------------------------------------

