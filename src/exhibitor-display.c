// file: exhibitor-display.c
// vim: tabstop=4 expandtab colorcolumn=81 list

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
#include <string.h>

//------------------------------------------------------------------------------
// PRIVATE

/// Check validity of the display.
bool noia_display_is_valid(NoiaDisplay* self)
{
    if (!self) {
        LOG_ERROR("Invalid display!");
        return false;
    }

    if (!self->output) {
        LOG_ERROR("Invalid output!");
        return false;
    }

    if (!self->workspace) {
        LOG_ERROR("Invalid workspace!");
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------

/// Setup display loop.
NoiaResult noia_display_setup(NoiaDisplay* self)
{
    // Setup the thread
    char name[32];
    snprintf(name, sizeof(name), "noia@%s", self->output->unique_name);
    noia_environment_on_enter_new_thread(self->thread, name);
    LOG_INFO1("Threads: starting display loop '%s'", name);

    // Initialize output
    /// @todo Squash this to one function call.
    LOG_INFO1("Initializing output '%s'", self->output->unique_name);
    self->output->renderer = self->output->initialize(self->output,
                                                      self->output->area.size);
    NoiaResult result = noia_output_initialize_rendering(self->output);

    if (result == NOIA_RESULT_SUCCESS) {
        // Read in background image
        /// @todo Reimplement background as surface.
        self->background =
              noia_image_read(noia_config()->background_image_path);
    }

    return result;
}

//------------------------------------------------------------------------------

/// Prepare rendering context for drawing layout
/// (currently only pointer and background).
void noia_display_setup_layout_context(NoiaDisplay* self,
                                       NoiaLayoutContext* context)
{
    memset(context, 0, sizeof(NoiaLayoutContext));

    // Setup pointer position and surface id
    NoiaPosition position = noia_exhibitor_pointer_get_global_position();
    if (noia_position_is_inside(position, self->output->area)) {
        context->pointer.pos.x = position.x - self->output->area.pos.x;
        context->pointer.pos.y = position.y - self->output->area.pos.y;
        context->pointer.sid = noia_exhibitor_pointer_get_sid();
    } else {
        context->pointer.pos.x = INT_MIN;
        context->pointer.pos.y = INT_MIN;
        context->pointer.sid = scInvalidSurfaceId;
    }

    // Setup background image
    context->background_buffer = self->background;
    context->background_transform = noia_config()->background_image_transform;
    context->background_color = noia_config()->background_color;
}

//------------------------------------------------------------------------------

/// Prepare context for drawing the whole scene and pass it to the renderer.
/// @todo Is order of calling renderer method for GL renderer correct?
void noia_display_redraw_all(NoiaDisplay* self)
{
    noia_frame_to_array(self->workspace, self->visible_surfaces);

    noia_exhibitor_pointer_update_hover_state(self->output,
                                              self->visible_surfaces);

    NoiaLayoutContext layout_context;
    noia_display_setup_layout_context(self, &layout_context);

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
    if (!noia_display_is_valid(self)) {
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

NoiaDisplay* noia_display_new(NoiaOutput* output, NoiaFrame* workspace)
{
    NoiaDisplay* self = malloc(sizeof(NoiaDisplay));
    if (!self) {
        LOG_ERROR("Could not create new display!");
        return self;
    }

    self->output = output;
    self->workspace = workspace;
    self->visible_surfaces = noia_pool_create(8, sizeof(NoiaSurfaceContext));
    noia_buffer_clean(&self->background);
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

    noia_pool_destroy(self->visible_surfaces);
    noia_object_unref((NoiaObject*) self->output);

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
    self->run = false;
    pthread_join(self->thread, NULL);
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

