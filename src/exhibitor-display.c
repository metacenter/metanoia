// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-display.h"
#include "exhibitor-pointer.h"

#include "utils-log.h"
#include "utils-time.h"
#include "utils-image.h"
#include "utils-environment.h"
#include "event-signals.h"
#include "event-loop.h"
#include "surface-coordinator.h"
#include "config.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------
// PRIVATE

/// @see NoiaDisplay
struct NoiaDisplayStruct {
    /// An output to draw on.
    NoiaOutput* output;

    /// Pointer to frame which represents active workspace.
    NoiaFrame* frame;

    /// Memory pool for storing visible surfaces.
    /// @see noia_display_redraw_all
    NoiaPool* visible_surfaces;

    /// Background surface ID.
    NoiaSurfaceId background_sid;

    /// Reference to exhibitor.
    NoiaExhibitor* exhibitor;

    /// Reference to coordinator.
    NoiaCoordinator* coordinator;

    /// Event dispatcher.
    /// @see noia_output_register_vblank_listener
    NoiaEventDispatcher* ed;

    /// Event loop.
    NoiaLoop* loop;

    /// Event loop task for display setup.
    NoiaTask* setup_task;

    /// Event loop task for display redraw.
    NoiaTask* redraw_task;

    /// Holds information if redraw is needed to avoid unnecessary redraws.
    bool redraw_needed;
};

//------------------------------------------------------------------------------
// PRIVATE

/// Check validity of the display.
bool noia_display_is_valid(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return false);
    NOIA_ENSURE(self->output, return false);
    NOIA_ENSURE(self->frame, return false);
    return true;
}

//------------------------------------------------------------------------------

/// Handle notification about vblank from output.
void noia_display_handle_vblank_notification(void* data)
{
    NoiaDisplay* display = data;
    NOIA_ENSURE(display, return);

    noia_object_ref((NoiaObject*) display->redraw_task);
    noia_loop_schedule_task(display->loop, display->redraw_task);
}

//------------------------------------------------------------------------------

/// Setup display loop.
NoiaResult noia_display_setup(NoiaDisplay* self)
{
    NoiaResult result = NOIA_RESULT_ERROR;

    LOG_INFO1("Initializing output '%s'", noia_output_get_name(self->output));

    // Initialize output
    result = noia_output_initialize_rendering(self->output);

    if (result == NOIA_RESULT_SUCCESS) {
        // Register vblank listener
        noia_output_register_vblank_listener
                                      (self->output, self->ed, self,
                                       noia_display_handle_vblank_notification);

        // Read in background image
        NoiaBuffer buff = noia_image_read(noia_config()->background_image_path);
        if (noia_buffer_is_valid(&buff)) {
            self->background_sid = noia_surface_create(self->coordinator);
            noia_surface_attach(self->coordinator,
                                self->background_sid,
                                buff.width, buff.height, buff.stride,
                                buff.data, NULL);
            noia_surface_commit(self->coordinator, self->background_sid);
            noia_buffer_release(&buff);
        }
    }

    return result;
}

//------------------------------------------------------------------------------

/// Prepare rendering context for drawing layout
/// (currently only pointer and background).
void noia_display_setup_layout_context(NoiaDisplay* self,
                                       NoiaPointer* pointer,
                                       NoiaLayoutContext* context)
{
    memset(context, 0, sizeof(NoiaLayoutContext));

    // Setup pointer position and surface id
    NoiaArea output_area = noia_output_get_area(self->output);
    NoiaPosition position = noia_exhibitor_pointer_get_global_position(pointer);
    if (noia_position_is_inside(position, output_area)) {
        context->pointer.pos.x = position.x - output_area.pos.x;
        context->pointer.pos.y = position.y - output_area.pos.y;
        context->pointer.sid = noia_exhibitor_pointer_get_sid(pointer);
    } else {
        context->pointer.pos.x = INT_MIN;
        context->pointer.pos.y = INT_MIN;
        context->pointer.sid = scInvalidSurfaceId;
    }

    // Setup background image
    context->background_sid = self->background_sid;
    context->background_transform = noia_config()->background_image_transform;
    context->background_color = noia_config()->background_color;
}

//------------------------------------------------------------------------------

/// Prepare context for drawing the whole scene and pass it to the renderer.
void noia_display_redraw_all(NoiaDisplay* self)
{
    // Check if there is something to do.
    if (not self->redraw_needed) {
        return;
    }

    // Prepare surfaces.
    noia_frame_to_array(noia_frame_get_last(self->frame),
                        self->visible_surfaces,
                        self->coordinator);

    // Prepare overlayer
    NoiaPointer* pointer = noia_exhibitor_get_pointer(self->exhibitor);
    noia_exhibitor_pointer_update_hover_state
          (pointer, noia_output_get_area(self->output), self->visible_surfaces);

    NoiaLayoutContext layout_context;
    noia_display_setup_layout_context(self, pointer, &layout_context);

    // Draw
    noia_output_draw(self->output,
                     self->coordinator,
                     self->visible_surfaces,
                     &layout_context);

    // Send notifications
    unsigned i;
    NoiaSurfaceContext* context;
    NOIA_ITERATE_POOL(self->visible_surfaces, i, context) {
        noia_event_signal_emit_int(SIGNAL_SCREEN_REFRESH, context->sid);
    }

    // Finish
    noia_pool_release(self->visible_surfaces);
    self->redraw_needed = false;
}

//------------------------------------------------------------------------------
// HANDLE TASKS

/// Helper function for setup task.
void noia_display_process_setup(void* edata NOIA_UNUSED, void* sdata)
{
    NoiaDisplay* display = sdata;
    NOIA_ENSURE(display, return);
    noia_display_setup(display);
}

//------------------------------------------------------------------------------

/// Helper function for redraw task.
void noia_display_process_redraw(void* edata NOIA_UNUSED, void* sdata)
{
    NoiaDisplay* display = sdata;
    NOIA_ENSURE(display, return);
    noia_display_redraw_all(display);
}

//------------------------------------------------------------------------------

/// Handle notification about needed redraw.
void noia_display_on_notify(void* edata NOIA_UNUSED, void* sdata)
{
    NoiaDisplay* display = sdata;
    NOIA_ENSURE(display, return);

    if (not display->redraw_needed) {
        display->redraw_needed = true;
        noia_output_schedule_page_flip(display->output);
    }
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaDisplay* noia_display_new(NoiaOutput* output,
                              NoiaFrame* frame,
                              NoiaExhibitor* exhibitor)
{
    NoiaDisplay* self = malloc(sizeof(NoiaDisplay));

    // Glue thread name
    char name[32];
    snprintf(name, sizeof(name), "noia@%s", noia_output_get_name(output));

    // Set display members
    self->output = output;
    self->frame = frame;
    self->loop = noia_loop_new(name);
    self->exhibitor = exhibitor;
    self->coordinator = noia_exhibitor_get_coordinator(exhibitor);
    self->ed = noia_exhibitor_get_dispatcher(exhibitor);
    self->visible_surfaces = noia_pool_create(8, sizeof(NoiaSurfaceContext));
    self->background_sid = scInvalidSurfaceId;
    self->redraw_needed = false;

    // Create tasks
    self->setup_task =
                noia_task_create(noia_display_process_setup, self->loop, self);
    self->redraw_task =
                noia_task_create(noia_display_process_redraw, self->loop, self);

    // Subscribe signals
    noia_event_signal_subscribe(SIGNAL_NOTIFY,
               noia_task_create(noia_display_on_notify,
                                self->loop, self));

    return self;
}

//------------------------------------------------------------------------------

void noia_display_free(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return);

    noia_loop_free(self->loop);
    noia_pool_destroy(self->visible_surfaces);
    noia_object_unref((NoiaObject*) self->output);
    noia_object_unref((NoiaObject*) self->setup_task);
    noia_object_unref((NoiaObject*) self->redraw_task);

    memset(self, 0, sizeof(NoiaDisplay));
    free(self);
}

//------------------------------------------------------------------------------

NoiaResult noia_display_start(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return NOIA_RESULT_ERROR);

    noia_loop_schedule_task(self->loop, self->setup_task);
    return noia_loop_run(self->loop);
}

//------------------------------------------------------------------------------

void noia_display_stop(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return);
    noia_loop_stop(self->loop);
    noia_loop_join(self->loop);
}

//------------------------------------------------------------------------------

NoiaArea noia_display_get_area(NoiaDisplay* self)
{
    return noia_output_get_area(self->output);
}

//------------------------------------------------------------------------------

int noia_display_compare_name(NoiaDisplay* self,
                              const char* name)
{
    NOIA_ENSURE(name, return -1);
    NOIA_ENSURE(self, return -1);
    NOIA_ENSURE(self->output, return -1);
    return strcmp(noia_output_get_name(self->output), name);
}

//------------------------------------------------------------------------------

