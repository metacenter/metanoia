// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor.h"
#include "exhibitor-display.h"
#include "exhibitor-strategist.h"
#include "exhibitor-pointer.h"

#include "utils-log.h"
#include "surface-coordinator.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

/// @see NoiaExhibitor
struct NoiaExhibitorStruct {
    NoiaList* displays;
    NoiaList* surface_history;
    NoiaCompositor* compositor;
    NoiaStrategist* strategist;
    NoiaPointer* pointer;
    NoiaCoordinator* coordinator;
};

//------------------------------------------------------------------------------

NoiaExhibitor* noia_exhibitor_new(NoiaCoordinator* coordinator)
{
    NoiaExhibitor* self = malloc(sizeof(NoiaExhibitor));

    self->coordinator = coordinator;
    self->surface_history = noia_list_new(NULL);
    self->displays = noia_list_new((NoiaFreeFunc) noia_display_free);
    self->compositor = noia_compositor_new(self);
    self->strategist = noia_strategist_new();
    self->pointer = noia_exhibitor_pointer_new(self);

    return self;
}

//------------------------------------------------------------------------------

void noia_exhibitor_free(NoiaExhibitor* self)
{
    NOIA_ENSURE(self, return);

    FOR_EACH(self->displays, link) {
        NoiaDisplay* display = (NoiaDisplay*) link->data;
        noia_display_stop(display);
    }

    noia_exhibitor_pointer_free(self->pointer);
    noia_strategist_free(self->strategist);
    noia_compositor_free(self->compositor);

    noia_list_free(self->displays);
    noia_list_free(self->surface_history);

    memset(self, 0, sizeof(NoiaExhibitor));
    free(self);
}

//------------------------------------------------------------------------------

void noia_exhibitor_setup(NoiaExhibitor* self)
{
    NOIA_ENSURE(self, return);

    noia_exhibitor_pointer_setup(self->pointer);
}

//------------------------------------------------------------------------------

NoiaList* noia_exhibitor_get_displays(NoiaExhibitor* self)
{
    return self->displays;
}

//------------------------------------------------------------------------------

NoiaList* noia_exhibitor_get_surface_history(NoiaExhibitor* self)
{
    return self->surface_history;
}

//------------------------------------------------------------------------------

NoiaCompositor* noia_exhibitor_get_compositor(NoiaExhibitor* self)
{
    return self->compositor;
}

//------------------------------------------------------------------------------

NoiaPointer* noia_exhibitor_get_pointer(NoiaExhibitor* self)
{
    return self->pointer;
}

//------------------------------------------------------------------------------

NoiaCoordinator* noia_exhibitor_get_coordinator(NoiaExhibitor* self)
{
    return self->coordinator;
}

//------------------------------------------------------------------------------

void noia_exhibitor_execute(NoiaExhibitor* self, NoiaCommand* command)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(command, return);
    noia_compositor_execute(self->compositor, command);
}

//------------------------------------------------------------------------------

void noia_exhibitor_create_new_display(NoiaExhibitor* exhibitor,
                                       NoiaOutput* output)
{
    NOIA_ENSURE(exhibitor, return);
    NOIA_ENSURE(output, return);

    NoiaFrame* display_frame =
                       noia_compositor_create_new_display(exhibitor->compositor,
                                                          output->area,
                                                          output->unique_name);

    noia_compositor_create_next_workspace(exhibitor->compositor, display_frame);
    NoiaDisplay* display = noia_display_new(output, display_frame, exhibitor);

    noia_list_append(exhibitor->displays, display);
    noia_display_start(display);
}

//------------------------------------------------------------------------------

void noia_exhibitor_destroy_display(NoiaExhibitor* exhibitor,
                                    NoiaOutput* output)
{
    NOIA_ENSURE(exhibitor, return);
    NOIA_ENSURE(output, return);

    LOG_INFO2("Stopping rendering thread for output '%s'", output->unique_name);

    FOR_EACH(exhibitor->displays, link) {
        NoiaDisplay* display = (NoiaDisplay*) link->data;
        if (noia_display_compare_name(display, output->unique_name) == 0) {
            noia_display_stop(display);
            break;
        }
    }

    noia_list_remove(exhibitor->displays, output->unique_name,
                     (NoiaCompareFunc) noia_display_compare_name);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_surface_ready(NoiaExhibitor* self,
                                     NoiaSurfaceId sid)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->strategist, return);
    NOIA_ENSURE(self->strategist->on_surface_ready, return);
    self->strategist->on_surface_ready(self, sid);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_surface_destroyed(NoiaExhibitor* self,
                                         NoiaSurfaceId sid)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->strategist, return);
    NOIA_ENSURE(self->strategist->on_surface_destroyed, return);

    self->strategist->on_surface_destroyed(self, sid);

    noia_compositor_unmanage_surface(self->compositor, sid);
    noia_exhibitor_pointer_on_surface_destroyed(self->pointer, sid);

    noia_list_remove(self->surface_history, (void*) sid,
                    (NoiaCompareFunc) noia_surface_compare);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_surface_change(NoiaExhibitor* self, NoiaSurfaceId sid)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_surface_change(self->pointer, sid);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_motion_x(NoiaExhibitor* self, int value)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_motion_x(self->pointer, self, value);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_motion_y(NoiaExhibitor* self, int value)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_motion_y(self->pointer, self, value);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_position_reset(NoiaExhibitor* self)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_position_reset(self->pointer);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_position_x(NoiaExhibitor* self, int value)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_position_x(self->pointer, self, value);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_position_y(NoiaExhibitor* self, int value)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_position_y(self->pointer, self, value);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_button(NoiaExhibitor* self,
                              uint32_t time   NOIA_UNUSED,
                              uint32_t button NOIA_UNUSED,
                              uint32_t state  NOIA_UNUSED)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_button(self->pointer, self);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_keyboard_focus_changed(NoiaExhibitor* self,
                                              NoiaSurfaceId new_sid)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_keyboard_focus_changed(self->pointer, new_sid);
}

//------------------------------------------------------------------------------

