// file: exhibitor-strategist.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-strategist.h"

#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------
// DEFAULT STRATEGIES

void noia_strategist_default_on_surface_ready(NoiaExhibitor* exhibitor,
                                              NoiaCompositor* compositor,
                                              NoiaSurfaceId sid)
{
    NOIA_ENSURE(exhibitor, return);
    NOIA_ENSURE(compositor, return);

    // Put surface on current workspace on current display
    if (noia_compositor_manage_surface(compositor, sid)) {
        NoiaList* history = noia_exhibitor_get_surface_history(exhibitor);
        noia_list_prepend(history, (void*) sid);
    }

    /// @todo Focus changing should be done in compositor strategy.
    noia_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED, sid);
}

//------------------------------------------------------------------------------

void noia_strategist_default_on_surface_destroyed
                                          (NoiaExhibitor* exhibitor NOIA_UNUSED,
                                           NoiaSurfaceId sid        NOIA_UNUSED)
{
    /// @todo Move to compositor strategy
    noia_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED,
                               scInvalidSurfaceId);
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaStrategist* noia_strategist_new(void)
{
    NoiaStrategist* self = malloc(sizeof(NoiaStrategist));
    NOIA_ENSURE(self, abort());
    memset(self, 0, sizeof(NoiaStrategist));

    noia_strategist_setup(self,
                          noia_strategist_default_on_surface_ready,
                          noia_strategist_default_on_surface_destroyed);

    return self;
}

//------------------------------------------------------------------------------

void noia_strategist_free(NoiaStrategist* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(NoiaStrategist));
    free(self);
}

//------------------------------------------------------------------------------

void noia_strategist_setup
                     (NoiaStrategist* self,
                      NoiaStrategistOnSurfaceReadyFunc     on_surface_ready,
                      NoiaStrategistOnSurfaceDestroyedFunc on_surface_destroyed)
{
    NOIA_ENSURE(self, return);
    self->on_surface_ready     = on_surface_ready;
    self->on_surface_destroyed = on_surface_destroyed;
}

//------------------------------------------------------------------------------

