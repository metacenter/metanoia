// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

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
    NoiaCommand* command = (NoiaCommand*) edata;
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    noia_exhibitor_execute(exhibitor, command);
    noia_object_unref(&command->base);
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

/// Handle motion in X-axis notification.
void noia_exhibitor_module_on_motion_x(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    int value = noia_int_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_motion_x(exhibitor, value);
}

//------------------------------------------------------------------------------

/// Handle motion in Y-axis notification.
void noia_exhibitor_module_on_motion_y(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    int value = noia_int_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_motion_y(exhibitor, value);
}

//------------------------------------------------------------------------------

/// Handle position reset notification.
void noia_exhibitor_module_on_position_reset(void* edata NOIA_UNUSED,
                                             void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    noia_exhibitor_on_position_reset(exhibitor);
}

//------------------------------------------------------------------------------

/// Handle position on X-axis notification.
void noia_exhibitor_module_on_position_x(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    int value = noia_int_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_position_x(exhibitor, value);
}

//------------------------------------------------------------------------------

/// Handle position on Y-axis notification.
void noia_exhibitor_module_on_position_y(void* edata, void* sdata)
{
    NoiaExhibitor* exhibitor = (NoiaExhibitor*) sdata;
    int value = noia_int_unref_get((NoiaIntObject*) edata);
    noia_exhibitor_on_position_y(exhibitor, value);
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

void noia_exhibitor_initialize(NoiaLoop* this_loop,
                               NoiaCoordinator* coordinator)
{
    NOIA_ENSURE(this_loop, return);

    NoiaExhibitor* exhibitor = noia_exhibitor_new(coordinator);
    noia_exhibitor_setup(exhibitor);

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
    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_X,
               noia_task_create(noia_exhibitor_module_on_motion_x,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_Y,
               noia_task_create(noia_exhibitor_module_on_motion_y,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_POINTER_POSITION_RESET,
               noia_task_create(noia_exhibitor_module_on_position_reset,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_POINTER_POSITION_X,
               noia_task_create(noia_exhibitor_module_on_position_x,
                                this_loop, exhibitor));

    noia_event_signal_subscribe(SIGNAL_POINTER_POSITION_Y,
               noia_task_create(noia_exhibitor_module_on_position_y,
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

