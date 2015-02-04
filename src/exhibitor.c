// file: exhibitor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor.h"
#include "exhibitor-strategist.h"
#include "exhibitor-pointer.h"

#include "utils-log.h"
#include "event-signals.h"
#include "global-objects.h"
#include "surface-manager.h"

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

void aura_exhibitor_create_new_display(AuraOutput* output)
{
    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
    AuraDisplay* display = aura_display_new(output);

    chain_append(exhibitor->displays, display);
    aura_display_start(display);

    if (!exhibitor->display) {
        exhibitor->display = display;
    }
}

//------------------------------------------------------------------------------

void aura_exhibitor_on_display_found(void* data)
{
    AuraOutput* output = (AuraOutput*) data;
    if (!output) {
        LOG_ERROR("Invalid output!");
        return;
    }
    if (!output->renderer) {
        LOG_ERROR("Invalid renderer!");
        return;
    }

    LOG_INFO1("Adding new renderer!");
    output->renderer->initialize(output->renderer);
    aura_exhibitor_create_new_display(output);
}

//------------------------------------------------------------------------------

void aura_exhibitor_on_display_lost(void* data)
{
    AuraOutput* output = (AuraOutput*) data;
    if (!output) {
        LOG_ERROR("Invalid output");
        return;
    }

    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();

    LOG_INFO2("Deleting renderer timer");
    Link* link;
    for (link = exhibitor->displays->first; link; link = link->next) {
        AuraDisplay* display = (AuraDisplay*) link->data;
        if (display && display->output == output) {
            aura_display_stop(display);
            // TODO: Remove display
            break;
        }
    }
}

//------------------------------------------------------------------------------

void aura_exhibitor_on_surface_created(void* data)
{
    AuraSurfaceId sid = aura_uint_unref_get((AuraIntObject*) data);
    if (sid == scInvalidSurfaceId) {
        return;
    }

    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
    exhibitor->priv->strategist->on_surface_created(exhibitor, sid);
}

//------------------------------------------------------------------------------

void aura_exhibitor_on_surface_destroyed(void* data)
{
    AuraSurfaceId sid = aura_uint_unref_get((AuraIntObject*) data);
    if (sid == scInvalidSurfaceId) {
        return;
    }

    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
    exhibitor->priv->strategist->on_surface_destroyed(exhibitor, sid);
    chain_remove(exhibitor->surface_history, (void*) sid,
                 (AuraCompareFunc) aura_surface_compare);
}

//------------------------------------------------------------------------------

void aura_exhibitor_pop_surface(AuraSurfaceId sid)
{
    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
    chain_remove(exhibitor->surface_history,
                (void*) sid, (AuraCompareFunc) aura_surface_compare);
    chain_append(exhibitor->surface_history, (void*) sid);

    AuraSurfaceData* surface = aura_surface_get(sid);
    if (surface) {
        aura_compositor_pop_surface(surface->group.compositor, sid);
    }
}

//------------------------------------------------------------------------------

void aura_exhibitor_pop_history_surface(int position)
{
    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();

    Link* link = NULL;
    AuraSurfaceId sid = scInvalidSurfaceId;
    if (position < 0) {
        int i = 1;
        link = exhibitor->surface_history->first;
        for (; link && i < -position; ++i, link = link->next);
    } else {
        int i = 0;
        link = exhibitor->surface_history->last;
        for (; link && i < position; ++i, link = link->prev);
    }

    if (link) {
        sid = (AuraSurfaceId) link->data;
    }
    if (sid == scInvalidSurfaceId) {
        return;
    }

    aura_exhibitor_pop_surface(sid);
}

//------------------------------------------------------------------------------

void aura_exhibitor_command_position(AuraArgmandType type,
                                     AuraArgmandType direction,
                                     int magnitude)
{
    AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
    aura_display_command_position(exhibitor->display,
                                  type, direction, magnitude);
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

    aura_event_signal_subscribe(SIGNAL_SURFACE_DESTROYED,
         aura_task_create(aura_exhibitor_on_surface_destroyed, this_loop));

    aura_exhibitor_pointer_initialize(this_loop);
}

//------------------------------------------------------------------------------

