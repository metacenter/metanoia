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
#include <string.h>

//------------------------------------------------------------------------------
// PRIVATE

bool noia_display_is_valid(NoiaDisplay* self)
{
    if (!self) {
        LOG_ERROR("Invalid display!");
        return false;
    }

    if (!self->compositor) {
        LOG_ERROR("No current compositor!");
        return false;
    }

    if (!self->output) {
        LOG_ERROR("Invalid output!");
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

void noia_display_redraw_all(NoiaDisplay* self)
{
    NoiaList* visible_surfaces =
                         noia_compositor_get_visible_surfaces(self->compositor);

    NoiaPosition pos = noia_exhibitor_pointer_get_position();

    self->output->renderer->draw(self->output->renderer, visible_surfaces,
                                pos.x, pos.y, noia_exhibitor_pointer_get_sid());

    if (self->output->begin_drawing) {
        self->output->begin_drawing(self->output);
    }

    if (self->output->renderer->swap_buffers) {
        self->output->renderer->swap_buffers(self->output->renderer);
    }

    if (self->output->end_drawing) {
        self->output->end_drawing(self->output);
    }

    noia_exhibitor_pointer_update_hover_state(visible_surfaces);

    // TODO: pass as list
    if (visible_surfaces) {
        FOR_EACH (visible_surfaces, link) {
            NoiaSurfaceId sid = (NoiaSurfaceId) link->data;
            noia_event_signal_emit_int(SIGNAL_SCREEN_REFRESH, sid);
        }
    }

    noia_list_free(visible_surfaces);
}

//------------------------------------------------------------------------------

void* noia_display_thread_loop(void* data)
{
    NoiaDisplay* self = (NoiaDisplay*) data;
    if (!noia_display_is_valid(self)) {
        return NULL;
    }

    char name[128];
    snprintf(name, sizeof name, "noia@%s", self->output->unique_name);
    noia_environment_on_enter_new_thread(self->thread, name);
    LOG_INFO1("Threads: starting display loop '%s'", name);

    LOG_INFO1("Initializing output '%s'", self->output->unique_name);
    self->output->renderer = self->output->initialize(self->output,
                                                      self->output->width,
                                                      self->output->height);

    NoiaResult result = noia_output_initialize_rendering(self->output);
    if (result == NOIA_RESULT_SUCCESS) {
        LOG_INFO1("Initialized new renderer!");
        self->run = true;
        while (self->run) {
            noia_surface_lock();
            noia_display_redraw_all(self);
            noia_surface_unlock();
            noia_event_timer_nanosleep(100 * 1000000);
        }
    } else {
        LOG_ERROR("Initializing new renderer failed!");
    }

    LOG_INFO1("Threads: stopped display loop '%s'", name);
    return NULL;
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaDisplay* noia_display_new(NoiaOutput* output)
{
    NoiaDisplay* self = malloc(sizeof(NoiaDisplay));
    if (!self) {
        LOG_ERROR("Could not create new display!");
        return self;
    }

    self->output = output;
    self->compositor = noia_compositor_new();
    self->compositors = noia_list_new((NoiaFreeFunc) noia_compositor_free);
    noia_list_append(self->compositors, self->compositor);
    return self;
}

//------------------------------------------------------------------------------

void noia_display_free(NoiaDisplay* self)
{
    if (!self) {
        return;
    }

    if (self->run) {
        LOG_ERROR("Trying to free running display!");
        return;
    }

    noia_object_unref((NoiaObject*) self->output);
    noia_list_free(self->compositors);

    memset(self, 0, sizeof(NoiaDisplay));
    free(self);
}

//------------------------------------------------------------------------------

int noia_display_start(NoiaDisplay* self)
{
    if (!self) {
        return -1;
    }

    return pthread_create(&self->thread, NULL, noia_display_thread_loop, self);
}

//------------------------------------------------------------------------------

void noia_display_stop(NoiaDisplay* self)
{
    if (!self) {
        return;
    }
    self->run = 0;
    pthread_join(self->thread, NULL);
}

//------------------------------------------------------------------------------

void noia_display_command_position(NoiaDisplay* self,
                                   NoiaArgmandType type,
                                   NoiaArgmandType direction,
                                   int position)
{
    noia_compositor_command_position(self->compositor,
                                     type, direction, position);
}

//------------------------------------------------------------------------------

int noia_display_compare_unique_name(NoiaDisplay* self,
                                     char* unique_name)
{
    if (!self || !self->output || !self->output->unique_name) {
        return -1;
    }
    return strcmp(self->output->unique_name, unique_name);
}

//------------------------------------------------------------------------------

