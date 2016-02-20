// file: exhibitor-module.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor.h"
#include "exhibitor-module.h"

#include "utils-log.h"
#include "event-signals.h"
#include "global-objects.h"

//------------------------------------------------------------------------------

/// Execute an action on selected frame.
/// @see NoiaAction
void noia_exhibitor_module_execute(void* edata, void* sdata)
{
    NoiaAction* action = (NoiaAction*) edata;
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    noia_exhibitor_execute(exhibitor, action);
    noia_object_unref(&action->base);
}

//------------------------------------------------------------------------------

/// Handle display found notification.
void noia_exhibitor_module_on_display_found(void* edata, void* sdata)
{
    NoiaOutput* output = (NoiaOutput*) edata;
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    noia_exhibitor_create_new_display(exhibitor, output);
}

//------------------------------------------------------------------------------

/// Handle display lost notification.
void noia_exhibitor_module_on_display_lost(void* edata, void* sdata)
{
    NoiaOutput* output = (NoiaOutput*) edata;
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    noia_exhibitor_destroy_display(exhibitor, output);
}

//------------------------------------------------------------------------------

/// Handle creation of new surface notification.
void noia_exhibitor_module_on_surface_ready(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_surface_ready(exhibitor, sid);
}

//------------------------------------------------------------------------------

/// Handle destruction of surface notification.
void noia_exhibitor_module_on_surface_destroyed(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_surface_destroyed(exhibitor, sid);
}

//------------------------------------------------------------------------------

/// Handle change cursor surface notification.
void noia_exhibitor_module_on_surface_change(void* edata,
                                             void* sdata NOIA_UNUSED)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_surface_change(exhibitor, sid);
}

//------------------------------------------------------------------------------

/// Handle motion reset notification.
void noia_exhibitor_module_on_motion_reset(void* edata NOIA_UNUSED, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    noia_exhibitor_on_motion_reset(exhibitor);
}

//------------------------------------------------------------------------------

/// Handle motion in X-axis notification.
void noia_exhibitor_module_on_motion_x(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    int abs_value = noia_int_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_motion_x(exhibitor, abs_value);
}

//------------------------------------------------------------------------------

/// Handle motion in Y-axis notification.
void noia_exhibitor_module_on_motion_y(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    int abs_value = noia_int_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_motion_y(exhibitor, abs_value);
}

//------------------------------------------------------------------------------

/// Handle button press or release notification.
void noia_exhibitor_module_on_button(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    NoiaButtonObject* object = (NoiaButtonObject*) edata;
    noia_exhibitor_on_button(exhibitor,
                             object->buttondata.time,
                             object->buttondata.code,
                             object->buttondata.value);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

/// Handle change of keyboard focus.
void noia_exhibitor_module_on_keyboard_focus_changed(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_keyboard_focus_changed(exhibitor, sid);
}

//------------------------------------------------------------------------------

/// Finalize the loop - realise resources.
void noia_exhibitor_module_finalize(void* edata NOIA_UNUSED, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    noia_event_signal_unsubscribe(exhibitor);
    noia_exhibitor_free(exhibitor);
}

//------------------------------------------------------------------------------
// PUBLIC

void noia_exhibitor_initialize(NoiaLoop* this_loop)
{
    NOIA_ENSURE(this_loop, return);

    NoiaExhibitor* exhibitor = noia_exhibitor_new();

    // actions
    noia_event_signal_subscribe(SIGNAL_ACTION,
               noia_task_create(noia_exhibitor_module_execute,
                                this_loop, exhibitor));

    // outputs
    noia_event_signal_subscribe(SIGNAL_OUTPUT_FOUND,
               noia_task_create(noia_exhibitor_module_on_display_found,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_OUTPUT_LOST,
               noia_task_create(noia_exhibitor_module_on_display_lost,
                                this_loop, exhibitor));

    // surfaces
    noia_event_signal_subscribe(SIGNAL_SURFACE_READY,
               noia_task_create(noia_exhibitor_module_on_surface_ready,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_SURFACE_DESTROYED,
               noia_task_create(noia_exhibitor_module_on_surface_destroyed,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_CURSOR_SURFACE_CHANGE,
               noia_task_create(noia_exhibitor_module_on_surface_change,
                                this_loop, exhibitor));

    // pointer
    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_RESET,
               noia_task_create(noia_exhibitor_module_on_motion_reset,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_X,
               noia_task_create(noia_exhibitor_module_on_motion_x,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_Y,
               noia_task_create(noia_exhibitor_module_on_motion_y,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_POINTER_BUTTON,
               noia_task_create(noia_exhibitor_module_on_button,
                                this_loop, exhibitor));

    // keyboard
    noia_event_signal_subscribe(SIGNAL_KEYBOARD_FOCUS_CHANGED,
               noia_task_create(noia_exhibitor_module_on_keyboard_focus_changed,
                                this_loop, exhibitor));

    // finalizer
    noia_loop_add_finalizer(this_loop,
                            noia_exhibitor_module_finalize,
                            exhibitor);
}

//------------------------------------------------------------------------------
