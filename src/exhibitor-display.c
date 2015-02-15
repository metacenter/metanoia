// file: exhibitor-display.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-display.h"
#include "exhibitor-pointer.h"
#include "utils-log.h"
#include "utils-list.h"
#include "utils-environment.h"
#include "event-timer.h"
#include "event-signals.h"
#include "surface-manager.h"

#include <malloc.h>
#include <memory.h>

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
    AuraList* visible_surfaces =
                         aura_compositor_get_visible_surfaces(self->compositor);

    AuraPosition pos = aura_exhibitor_pointer_get_position();

    self->output->renderer->draw(self->output->renderer, visible_surfaces,
                                pos.x, pos.y, aura_exhibitor_pointer_get_sid());

    aura_exhibitor_pointer_update_hover_state(visible_surfaces);

    // TODO: pass as list
    if (visible_surfaces) {
        FOR_EACH (visible_surfaces, link) {
            AuraSurfaceId sid = (AuraSurfaceId) link->data;
            aura_event_signal_emit_int(SIGNAL_SCREEN_REFRESH, sid);
        }
    }

    aura_list_free(visible_surfaces);
}

//------------------------------------------------------------------------------

void* aura_display_thread_loop(void* data)
{
    AuraDisplay* self = (AuraDisplay*) data;
    if (!aura_display_is_valid(self)) {
        return NULL;
    }

    char* name = "aura@";
    aura_environment_on_enter_new_thread(self->thread, name);
    LOG_INFO1("Threads: starting display loop '%s'", name);

    self->run = 1;
    while (self->run) {
        aura_surface_lock();
        aura_display_redraw_all(self);
        aura_surface_unlock();
        aura_event_timer_nanosleep(100 * 1000000);
    }

    LOG_INFO1("Threads: stopped display loop '%s'", name);
    return NULL;
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
    self->compositors = aura_list_new((AuraFreeFunc) aura_compositor_free);
    aura_list_append(self->compositors, self->compositor);
    return self;
}

//------------------------------------------------------------------------------

void aura_display_free(AuraDisplay* self)
{
    if (!self) {
        return;
    }

    if (self->run) {
        LOG_ERROR("Trying to free running display!");
        return;
    }

    aura_object_unref((AuraObject*) self->output);
    aura_list_free(self->compositors);

    memset(self, 0, sizeof(AuraDisplay));
    free(self);
}

//------------------------------------------------------------------------------

int aura_display_start(AuraDisplay* self)
{
    if (!self) {
        return -1;
    }

    return pthread_create(&self->thread, NULL, aura_display_thread_loop, self);
}

//------------------------------------------------------------------------------

void aura_display_stop(AuraDisplay* self)
{
    if (!self) {
        return;
    }
    self->run = 0;
    pthread_join(self->thread, NULL);
}

//------------------------------------------------------------------------------

void aura_display_command_position(AuraDisplay* self,
                                   AuraArgmandType type,
                                   AuraArgmandType direction,
                                   int position)
{
    aura_compositor_command_position(self->compositor,
                                     type, direction, position);
}

//------------------------------------------------------------------------------

