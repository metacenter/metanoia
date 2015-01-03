// file: exhibitor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor.h"
#include "exhibitor-strategist.h"

#include "utils-log.h"
#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

struct AuraExhibitorPriv {
    AuraStrategist* strategist;
};

//------------------------------------------------------------------------------

AuraExhibitor* aura_exhibitor_get_instance()
{
    static AuraExhibitor* exhibitor = 0;
    if (exhibitor) {
        return exhibitor;
    }

    exhibitor = malloc(sizeof(AuraExhibitor));
    memset(exhibitor, 0, sizeof(AuraExhibitor));

    exhibitor->priv = malloc(sizeof(AuraExhibitorPriv));
    memset(exhibitor->priv, 0, sizeof(AuraExhibitorPriv));

    exhibitor->surface_history = chain_new(0);
    exhibitor->displays = chain_new(0);
    exhibitor->priv->strategist = aura_strategist_create();

    return exhibitor;
}

//------------------------------------------------------------------------------

void aura_exhibitor_create_new_display(AuraRenderer* renderer)
{
    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
    AuraDisplay* display = aura_display_new(renderer);

    chain_append(exhibitor->displays, display);
    aura_display_start(display);

    if (!exhibitor->display) {
        exhibitor->display = display;
    }
}

//------------------------------------------------------------------------------

void aura_exhibitor_on_display_found(void* data)
{
    AuraRenderer* renderer = (AuraRenderer*) data;
    if (!renderer) {
        LOG_ERROR("Invalid renderer");
        return;
    }

    LOG_INFO1("Adding new renderer!");
    renderer->initialize((struct AuraRenderer*) renderer);
    aura_exhibitor_create_new_display(renderer);
}

//------------------------------------------------------------------------------

void aura_exhibitor_on_display_lost(void* data)
{
    AuraRenderer* renderer = (AuraRenderer*) data;
    if (!renderer) {
        LOG_ERROR("Invalid renderer");
        return;
    }

    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();

    LOG_INFO2("Deleting renderer timer");
    Link* link;
    for (link = exhibitor->displays->first; link; link = link->next) {
        AuraDisplay* display = (AuraDisplay*) link->data;
        if (display && display->renderer == renderer) {
            aura_display_stop(display);
            // TODO: Remove display
            break;
        }
    }
}

//------------------------------------------------------------------------------

void aura_exhibitor_on_surface_created(void* data)
{
    SurfaceId sid = (SurfaceId) data;
    if (sid == scInvalidSurfaceId) {
        return;
    }

    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
    exhibitor->priv->strategist->on_surface_created(exhibitor, sid);
}

//------------------------------------------------------------------------------

void aura_exhibitor_initialize(AuraLoop* this_loop)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    aura_event_signal_subscribe(SIGNAL_DISPLAY_FOUND,
         aura_task_create(aura_exhibitor_on_display_found, this_loop));

    aura_event_signal_subscribe(SIGNAL_DISPLAY_LOST,
         aura_task_create(aura_exhibitor_on_display_lost, this_loop));

    aura_event_signal_subscribe(SIGNAL_SURFACE_CREATED,
         aura_task_create(aura_exhibitor_on_surface_created, this_loop));
}

//------------------------------------------------------------------------------

