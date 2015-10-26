// file: exhibitor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor.h"
#include "exhibitor-strategist.h"
#include "exhibitor-pointer.h"

#include "utils-log.h"
#include "event-signals.h"
#include "global-objects.h"
#include "global-functions.h"
#include "surface-manager.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

struct NoiaExhibitorPriv {
    NoiaStrategist* strategist;
};

//------------------------------------------------------------------------------

NoiaExhibitor* noia_exhibitor_get_instance()
{
    static NoiaExhibitor exhibitor;
    if (exhibitor.priv) {
        return &exhibitor;
    }

    exhibitor.surface_history = noia_list_new(NULL);
    exhibitor.displays = noia_list_new((NoiaFreeFunc) noia_display_free);
    exhibitor.compositor = noia_compositor_new();

    exhibitor.priv = malloc(sizeof(NoiaExhibitorPriv));
    exhibitor.priv->strategist = noia_strategist_new();

    return &exhibitor;
}

//------------------------------------------------------------------------------

void noia_exhibitor_create_new_display(NoiaOutput* output)
{
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();

    NoiaFrame* workspace =
                    noia_compositor_create_new_workspace(exhibitor->compositor,
                                                         output->width,
                                                         output->height);

    NoiaDisplay* display = noia_display_new(output, workspace);

    noia_list_append(exhibitor->displays, display);
    noia_display_start(display);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_display_found(void* data)
{
    NoiaOutput* output = (NoiaOutput*) data;
    if (!output) {
        LOG_ERROR("Invalid output!");
        return;
    }

    noia_exhibitor_create_new_display(output);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_display_lost(void* data)
{
    NoiaOutput* output = (NoiaOutput*) data;
    if (!output) {
        LOG_ERROR("Invalid output");
        return;
    }

    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();

    LOG_INFO2("Stopping rendering thread for output '%s'", output->unique_name);

    FOR_EACH(exhibitor->displays, link) {
        NoiaDisplay* display = (NoiaDisplay*) link->data;
        if (display && display->output == output) {
            noia_display_stop(display);
            break;
        }
    }

    noia_list_remove(exhibitor->displays, output->unique_name,
                     (NoiaCompareFunc) noia_display_compare_unique_name);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_surface_ready(void* data)
{
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) data);
    if (sid == scInvalidSurfaceId) {
        return;
    }

    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
    exhibitor->priv->strategist->on_surface_ready(exhibitor, sid);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_surface_destroyed(void* data)
{
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) data);
    if (sid == scInvalidSurfaceId) {
        return;
    }

    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
    exhibitor->priv->strategist->on_surface_destroyed(exhibitor, sid);

    noia_compositor_unmanage_surface(exhibitor->compositor, sid);

    noia_list_remove(exhibitor->surface_history, (void*) sid,
                    (NoiaCompareFunc) noia_surface_compare);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pop_surface(NoiaSurfaceId sid)
{
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();

    noia_list_remove(exhibitor->surface_history,
                    (void*) sid, (NoiaCompareFunc) noia_surface_compare);
    noia_list_append(exhibitor->surface_history, (void*) sid);

    noia_compositor_pop_surface(exhibitor->compositor, sid);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pop_history_surface(int position)
{
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();

    Link* link = NULL;
    NoiaSurfaceId sid = scInvalidSurfaceId;
    if (position < 0) {
        int i = 1;
        link = noia_list_first(exhibitor->surface_history);
        for (; link && i < -position; ++i, link = link->next);
    } else {
        int i = 0;
        link = noia_list_last(exhibitor->surface_history);
        for (; link && i < position; ++i, link = link->prev);
    }

    if (link) {
        sid = (NoiaSurfaceId) link->data;
    }
    if (sid == scInvalidSurfaceId) {
        return;
    }

    noia_exhibitor_pop_surface(sid);
}

//------------------------------------------------------------------------------

void noia_exhibitor_command_position(NoiaArgmandType type,
                                     NoiaArgmandType direction,
                                     int magnitude)
{
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
    noia_compositor_command_position(exhibitor->compositor,
                                     type, direction, magnitude);
}

//------------------------------------------------------------------------------

void noia_exhibitor_command_anchorize()
{
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
    noia_compositor_command_anchorize(exhibitor->compositor);
}

//------------------------------------------------------------------------------

void noia_exhibitor_finalize(void* data NOIA_UNUSED)
{
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();

    noia_event_signal_unsubscribe(exhibitor);

    FOR_EACH (exhibitor->displays, link) {
        NoiaDisplay* display = (NoiaDisplay*) link->data;
        noia_display_stop(display);
    }

    noia_strategist_free(exhibitor->priv->strategist);
    memset(exhibitor->priv, 0, sizeof(NoiaExhibitorPriv));
    free(exhibitor->priv);

    noia_compositor_free(exhibitor->compositor);
    noia_list_free(exhibitor->displays);
    noia_list_free(exhibitor->surface_history);
    memset(exhibitor, 0, sizeof(NoiaExhibitor));
}

//------------------------------------------------------------------------------

void noia_exhibitor_initialize(NoiaLoop* this_loop)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();

    noia_event_signal_subscribe(SIGNAL_OUTPUT_FOUND,
               noia_task_create(noia_exhibitor_on_display_found,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_OUTPUT_LOST,
               noia_task_create(noia_exhibitor_on_display_lost,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_SURFACE_READY,
               noia_task_create(noia_exhibitor_on_surface_ready,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_SURFACE_DESTROYED,
               noia_task_create(noia_exhibitor_on_surface_destroyed,
                                this_loop, exhibitor));

    noia_loop_add_finalizer(this_loop, noia_exhibitor_finalize);

    noia_exhibitor_pointer_initialize(this_loop, exhibitor);
}

//------------------------------------------------------------------------------

