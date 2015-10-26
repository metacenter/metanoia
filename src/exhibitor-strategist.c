// file: exhibitor-strategist.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-strategist.h"

#include "utils-log.h"
#include "event-signals.h"

#include "malloc.h"
#include "memory.h"

//------------------------------------------------------------------------------

void noia_strategist_on_surface_ready(NoiaExhibitor* exhibitor,
                                      NoiaSurfaceId sid)
{
    if (!exhibitor || !exhibitor->compositor) {
        LOG_ERROR("Invalid Exhibitor!");
        return;
    }

    // Put surface on current workspace on current display
    if (noia_compositor_manage_surface(exhibitor->compositor, sid)) {
        noia_list_append(exhibitor->surface_history, (void*) sid);
    }

    /// @todo Focus changing should be done in compositor strategy.
    noia_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED, sid);
}

//------------------------------------------------------------------------------

void noia_strategist_on_surface_destroyed(NOIA_UNUSED NoiaExhibitor* exhibitor,
                                          NOIA_UNUSED NoiaSurfaceId sid)
{
    // TODO: move to compositor strategy
    noia_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED,
                               scInvalidSurfaceId);
}

//------------------------------------------------------------------------------

NoiaStrategist* noia_strategist_new()
{
    NoiaStrategist* self = malloc(sizeof(NoiaStrategist));
    if (!self) {
        LOG_ERROR("Could not create strategist!");
        return self;
    }

    memset(self, 0, sizeof(NoiaStrategist));

    self->on_surface_ready     = noia_strategist_on_surface_ready;
    self->on_surface_destroyed = noia_strategist_on_surface_destroyed;
    return self;
}

//------------------------------------------------------------------------------

void noia_strategist_free(NoiaStrategist* self)
{
    if (!self) {
        return;
    }

    memset(self, 0, sizeof(NoiaStrategist));
    free(self);
}

//------------------------------------------------------------------------------

