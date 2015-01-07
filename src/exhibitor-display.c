// file: exhibitor-display.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-display.h"
#include "exhibitor-compositor.h"
#include "exhibitor-pointer.h"
#include "utils-log.h"
#include "event-timer.h"
#include "event-signals.h"

#include <malloc.h>

//------------------------------------------------------------------------------
// PRIVATE

int aura_display_is_valid(AuraDisplay* self)
{
    if (!self) {
        LOG_ERROR("Invalid risplay!");
        return 0;
    }

    if (!self->compositor) {
        LOG_ERROR("No current compositor!");
        return 0;
    }

    if (!self->output) {
        LOG_ERROR("Invalid output!");
        return 0;
    }

    if (!self->output->renderer) {
        LOG_ERROR("Invalid renderer!");
        return 0;
    }

    if (!self->output->renderer->draw) {
        LOG_ERROR("Wrong renderer implementation!");
        return 0;
    }

    return 1;
}

//------------------------------------------------------------------------------

void aura_display_redraw_all(AuraDisplay* self)
{
    if (!aura_display_is_valid(self)) {
        return;
    }

    Chain* visible_surfaces =
                         aura_compositor_get_visible_surfaces(self->compositor);

    AuraPosition pos = aura_exhibitor_get_pointer_position();

    self->output->renderer->draw(self->output->renderer,
                                 visible_surfaces, pos.x, pos.y);

    // TODO: pass as list
    if (visible_surfaces) {
        Link* link;
        for (link = visible_surfaces->first; link; link = link->next) {
            SurfaceId sid = (SurfaceId) link->data;
            aura_event_signal_emit(SIGNAL_SCREEN_REFRESH, (void*) sid);
        }
    }

    // TODO: free visible surfaces
}

//------------------------------------------------------------------------------
// PUBLIC

AuraDisplay* aura_display_new(AuraOutput* output)
{
    AuraDisplay* self = malloc(sizeof(AuraDisplay));
    if (!self) {
        LOG_ERROR("Could not create new display!");
        return self;
    }

    self->output = output;
    self->compositor = aura_compositor_new();
    self->compositors = chain_new(0);
    chain_append(self->compositors, self->compositor);
    return self;
}

//------------------------------------------------------------------------------

void aura_display_start(AuraDisplay* self)
{
    self->timerid = aura_event_timer_run(100, // TODO: use displays frame rate
                              (AuraTimerHandler) aura_display_redraw_all, self);
}

//------------------------------------------------------------------------------

void aura_display_stop(AuraDisplay* self)
{
    aura_event_timer_delete(self->timerid);
}

//------------------------------------------------------------------------------

