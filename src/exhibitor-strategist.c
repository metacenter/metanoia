// file: exhibitor-strategist.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-strategist.h"

#include "utils-log.h"
#include "event-signals.h"

#include "malloc.h"
#include "memory.h"

//------------------------------------------------------------------------------

void aura_strategist_on_surface_created(AuraExhibitor* exhibitor,
                                        AuraSurfaceId sid)
{
    // Put surface on current workspace on current display
    aura_compositor_manage_surface(exhibitor->display->compositor, sid);

    chain_append(exhibitor->surface_history, (void*) sid);
    /// @todo Focus changing should be done in compositor strategy.
    aura_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED, sid);
}

//------------------------------------------------------------------------------

void aura_strategist_on_surface_destroyed(AURA_UNUSED AuraExhibitor* exhibitor,
                                          AURA_UNUSED AuraSurfaceId sid)
{
    // TODO: remove surface from workspace
    //aura_compositor_unmanage_surface(exhibitor->display->compositor, sid);

    // TODO: move to compositor strategy
    aura_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED,
                               scInvalidSurfaceId);
}

//------------------------------------------------------------------------------

AuraStrategist* aura_strategist_create()
{
    AuraStrategist* self = malloc(sizeof(AuraStrategist));
    if (!self) {
        LOG_ERROR("Could not create strategist!");
        return self;
    }

    memset(self, 0, sizeof(AuraStrategist));

    self->on_surface_created   = aura_strategist_on_surface_created;
    self->on_surface_destroyed = aura_strategist_on_surface_destroyed;
    return self;
}

//------------------------------------------------------------------------------
