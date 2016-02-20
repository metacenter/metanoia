// file: exhibitor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor.h"
#include "exhibitor-display.h"
#include "exhibitor-strategist.h"
#include "exhibitor-pointer.h"

#include "utils-log.h"
#include "surface-manager.h"

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
};

//------------------------------------------------------------------------------

NoiaExhibitor* noia_exhibitor_new(void)
{
    NoiaExhibitor* self = malloc(sizeof(NoiaExhibitor));

    self->surface_history = noia_list_new(NULL);
    self->displays = noia_list_new((NoiaFreeFunc) noia_display_free);
    self->compositor = noia_compositor_new(self);
    self->strategist = noia_strategist_new();
    self->pointer = noia_exhibitor_pointer_new();

    return self;
}

//------------------------------------------------------------------------------

void noia_exhibitor_free(NoiaExhibitor* self)
{
    NOIA_ENSURE(self, return);

    noia_exhibitor_pointer_free(self->pointer);
    noia_strategist_free(self->strategist);
    noia_compositor_free(self->compositor);
    noia_list_free(self->displays);
    noia_list_free(self->surface_history);

    memset(self, 0, sizeof(NoiaExhibitor));
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

void noia_exhibitor_execute(NoiaExhibitor* self, NoiaAction* action)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(action, return);
    noia_compositor_execute(self->compositor, action);
}

//------------------------------------------------------------------------------

void noia_exhibitor_create_new_display(NoiaExhibitor* exhibitor,
                                       NoiaOutput* output)
{
    NOIA_ENSURE(exhibitor, return);
    NOIA_ENSURE(output, return);

    NoiaFrame* workspace =
               noia_compositor_create_new_workspace(exhibitor->compositor,
                                                    output->area.size);

    NoiaDisplay* display = noia_display_new(output, workspace, exhibitor);

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
    self->strategist->on_surface_ready(self, self->compositor, sid);
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

void noia_exhibitor_on_motion_reset(NoiaExhibitor* self)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_motion_reset(self->pointer);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_motion_x(NoiaExhibitor* self, int abs_value)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_motion_x(self->pointer, self, abs_value);
}

//------------------------------------------------------------------------------

void noia_exhibitor_on_motion_y(NoiaExhibitor* self, int abs_value)
{
    NOIA_ENSURE(self, return);
    noia_exhibitor_pointer_on_motion_y(self->pointer, self, abs_value);
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

