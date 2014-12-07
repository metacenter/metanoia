// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils-log.h"
#include "utils-chain.h"
#include "event-timer.h"
#include "event-signals.h"
#include "surface-priv.h"

Chain* visible_surfaces = NULL;
AuraRenderer* renderer = NULL;

//------------------------------------------------------------------------------

static void aura_surface_manager_redraw_all()
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

// TBR
void aura_surface_manage(SurfaceId id)
{
    // TODO: finish this

    if (visible_surfaces == NULL) {
        visible_surfaces = chain_new(NULL);
    }

    chain_append(visible_surfaces, (void*) id);

    aura_event_signal_emit(SIGNAL_KEYBOARD_FOCUS_CHANGED, (void*) id);
}

//------------------------------------------------------------------------------

void aura_surface_attach_egl(SurfaceId id,
                             void* resource)
{
    SurfaceData* surface = aura_surface_get(id);
    if (surface == NULL) {
        LOG_WARN2("Could not find surface (id: %d)", id);
        return;
    }

    // TODO: log at init if renderer supports egl
    if (renderer && renderer->attach) {
        renderer->attach((struct AuraRenderer*) renderer, id, resource);
    }
}

//------------------------------------------------------------------------------

void aura_surface_commit(SurfaceId id,
                         int width,
                         int height,
                         int stride,
                         uint8_t* data)
{
    SurfaceData* surface = aura_surface_get(id);
    if (surface == 0) {
        LOG_WARN2("Could not find surface (id: %d)", id);
        return;
    }

    surface->buffer.width  = width;
    surface->buffer.height = height;
    surface->buffer.stride = stride;
    surface->buffer.data   = data;
}

//------------------------------------------------------------------------------

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

    aura_event_signal_subscribe(SIGNAL_DISPLAY_FOUND,
         aura_task_create(on_display_found, this_loop));
}

//------------------------------------------------------------------------------

