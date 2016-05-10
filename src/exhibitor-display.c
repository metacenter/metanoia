// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-display.h"
#include "exhibitor-pointer.h"

#include "utils-log.h"
#include "utils-image.h"
#include "utils-environment.h"
#include "event-timer.h"
#include "event-signals.h"
#include "surface-manager.h"
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

    /// State of the thread.
    bool run;

    /// Reference to exhibitor.
    NoiaExhibitor* exhibitor;

    /// Thread object.
    pthread_t thread;
};

//------------------------------------------------------------------------------

/// Check validity of the display.
bool noia_display_is_valid(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return false);
    NOIA_ENSURE(self->output, return false);
    NOIA_ENSURE(self->frame, return false);
    return true;
}
//------------------------------------------------------------------------------

/// Setup display loop.
NoiaResult noia_display_setup(NoiaDisplay* self)
{
    NoiaResult result = NOIA_RESULT_ERROR;

    // Setup the thread
    char name[32];
    snprintf(name, sizeof(name), "noia@%s", self->output->unique_name);
    noia_environment_on_enter_new_thread(self->thread, name);

    LOG_INFO1("Threads: starting display loop '%s'", name);
    LOG_INFO1("Initializing output '%s'", self->output->unique_name);

    NOIA_BLOCK {
        // Initialize output
        /// @todo Squash this to one function call.
        self->output->renderer =
                self->output->initialize(self->output, self->output->area.size);
        result = noia_output_initialize_rendering(self->output);
        NOIA_ASSERT_RESULT(result);

        // Read in background image
        NoiaBuffer buff = noia_image_read(noia_config()->background_image_path);
        self->background_sid = noia_surface_create();
        noia_surface_attach(self->background_sid,
                            buff.width, buff.height, buff.stride,
                            buff.data, NULL);
        noia_surface_commit(self->background_sid);

        result = NOIA_RESULT_SUCCESS;
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
    NoiaPosition position = noia_exhibitor_pointer_get_global_position(pointer);
    if (noia_position_is_inside(position, self->output->area)) {
        context->pointer.pos.x = position.x - self->output->area.pos.x;
        context->pointer.pos.y = position.y - self->output->area.pos.y;
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
/// @todo Is order of calling renderer method for GL renderer correct?
void noia_display_redraw_all(NoiaDisplay* self)
{
    noia_frame_to_array(noia_frame_get_last(self->frame),
                        self->visible_surfaces);

    NoiaPointer* pointer = noia_exhibitor_get_pointer(self->exhibitor);
    noia_exhibitor_pointer_update_hover_state(pointer,
                                              self->output->area,
                                              self->visible_surfaces);

    NoiaLayoutContext layout_context;
    noia_display_setup_layout_context(self, pointer, &layout_context);

    self->output->renderer->draw(self->output->renderer,
                                 self->visible_surfaces,
                                 &layout_context);

    if (self->output->renderer->swap_buffers) {
        self->output->renderer->swap_buffers(self->output->renderer);
    }

    if (self->output->begin_drawing) {
        self->output->begin_drawing(self->output);
    }

    if (self->output->end_drawing) {
        self->output->end_drawing(self->output);
    }

    unsigned i;
    NoiaSurfaceContext* context;
    NOIA_ITERATE_POOL(self->visible_surfaces, i, context) {
        noia_event_signal_emit_int(SIGNAL_SCREEN_REFRESH, context->sid);
    }

    noia_pool_release(self->visible_surfaces);
}

//------------------------------------------------------------------------------

/// Main loop of the display thread.
/// @todo Extend output to provide information about v-blanks.
void* noia_display_thread_loop(void* data)
{
    NoiaDisplay* self = (NoiaDisplay*) data;
    if (not noia_display_is_valid(self)) {
        return NULL;
    }

    if (noia_display_setup(self) == NOIA_RESULT_SUCCESS) {
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

    LOG_INFO1("Threads: stopped display loop");
    return NULL;
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaDisplay* noia_display_new(NoiaOutput* output,
                              NoiaFrame* frame,
                              NoiaExhibitor* exhibitor)
{
    NoiaDisplay* self = malloc(sizeof(NoiaDisplay));

    self->run = false;
    self->output = output;
    self->frame = frame;
    self->exhibitor = exhibitor;
    self->visible_surfaces = noia_pool_create(8, sizeof(NoiaSurfaceContext));
    self->background_sid = scInvalidSurfaceId;

    return self;
}

//------------------------------------------------------------------------------

void noia_display_free(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(not self->run, return);

    noia_pool_destroy(self->visible_surfaces);
    noia_object_unref((NoiaObject*) self->output);

    memset(self, 0, sizeof(NoiaDisplay));
    free(self);
}

//------------------------------------------------------------------------------

NoiaResult noia_display_start(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return NOIA_RESULT_ERROR);

    NoiaResult result = NOIA_RESULT_ERROR;
    int code = pthread_create(&self->thread, NULL,
                              noia_display_thread_loop, self);
    if (code == 0) {
        result = NOIA_RESULT_SUCCESS;
    } else {
        LOG_ERROR("Threads: error while creating thread: '%s'", strerror(code));
    }

    return result;
}

//------------------------------------------------------------------------------

void noia_display_stop(NoiaDisplay* self)
{
    NOIA_ENSURE(self, return);
    self->run = false;
    pthread_join(self->thread, NULL);
}

//------------------------------------------------------------------------------

NoiaArea noia_display_get_area(NoiaDisplay* self)
{
    return self->output->area;
}

//------------------------------------------------------------------------------

int noia_display_compare_name(NoiaDisplay* self,
                              const char* name)
{
    NOIA_ENSURE(name, return -1);
    NOIA_ENSURE(self, return -1);
    NOIA_ENSURE(self->output, return -1);
    NOIA_ENSURE(self->output->unique_name, return -1);
    return strcmp(self->output->unique_name, name);
}

//------------------------------------------------------------------------------

