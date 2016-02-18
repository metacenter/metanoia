// file: exhibitor-pointer.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-pointer.h"
#include "exhibitor.h"
#include "surface-manager.h"

#include "utils-log.h"
#include "global-macros.h"
#include "global-objects.h"
#include "global-macros.h"
#include "event-signals.h"

#define INVALID_POINTER_VALUE -1

/// @todo Do not store these variables as globals.
static NoiaSurfaceId cursor_sid = 0;
static NoiaPosition position = {100, 100};
static NoiaPosition last_abs = {INVALID_POINTER_VALUE, INVALID_POINTER_VALUE};
static NoiaPosition last_rel = {INVALID_POINTER_VALUE, INVALID_POINTER_VALUE};
static NoiaSurfaceId focused_sid = scInvalidSurfaceId;
static NoiaOutput* active_output = NULL;

pthread_mutex_t pointer_mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------
// PRIVATE

/// Find the output inside which is pointer.
/// If found - nothing to do.
/// If no output found - update cursor position with closest on inside
/// previously active output.
/// @see noia_exhibitor_pointer_on_motion_x, noia_exhibitor_pointer_on_motion_y
NoiaPosition noia_exhibitor_pointer_cast_into_output(NoiaPosition position)
{
    NOIA_BLOCK {
        if (active_output
        &&  noia_position_is_inside(position, active_output->area)) {
            // Nothing to do - pointer inside active output
            break;
        }

        bool found = false;
        NoiaList* displays = noia_exhibitor_get_displays();
        FOR_EACH (displays, link) {
            NoiaDisplay* display = (NoiaDisplay*) link->data;
            if (noia_position_is_inside(position, display->output->area)) {
                // Set new active output and exit
                active_output = display->output;
                found = true;
                break;
            }
        }

        if (!found && active_output) {
            // Pointer outside any known output - cast it to the last active
            position = noia_position_cast(position, active_output->area);
        }
    }

    return position;
}

//------------------------------------------------------------------------------

/// Handle motion reset notification.
void noia_exhibitor_pointer_on_motion_reset()
{
    pthread_mutex_lock(&pointer_mutex);

    last_abs.x = INVALID_POINTER_VALUE;
    last_abs.y = INVALID_POINTER_VALUE;

    pthread_mutex_unlock(&pointer_mutex);
}

//------------------------------------------------------------------------------

/// Handle motion in X-axis notification.
void noia_exhibitor_pointer_on_motion_x(void* edata, void* sdata NOIA_UNUSED)
{
    pthread_mutex_lock(&pointer_mutex);

    int abs_value = noia_int_unref_get((NoiaIntObject*) edata);
    if (last_abs.x != INVALID_POINTER_VALUE) {
        position.x += abs_value - last_abs.x;
        position = noia_exhibitor_pointer_cast_into_output(position);
    }
    last_abs.x = abs_value;

    pthread_mutex_unlock(&pointer_mutex);
}

//------------------------------------------------------------------------------

/// Handle motion in Y-axis notification.
void noia_exhibitor_pointer_on_motion_y(void* edata, void* sdata NOIA_UNUSED)
{
    pthread_mutex_lock(&pointer_mutex);

    int abs_value = noia_int_unref_get((NoiaIntObject*) edata);
    if (last_abs.y != INVALID_POINTER_VALUE) {
        position.y += abs_value - last_abs.y;
        position = noia_exhibitor_pointer_cast_into_output(position);
    }
    last_abs.y = abs_value;

    pthread_mutex_unlock(&pointer_mutex);
}

//------------------------------------------------------------------------------

/// Handle change surface notification.
void noia_exhibitor_pointer_on_surface_change(void* edata,
                                              void* sdata NOIA_UNUSED)
{
    pthread_mutex_lock(&pointer_mutex);

    NoiaSurfaceId sid = noia_int_unref_get((NoiaIntObject*) edata);
    NoiaSurfaceData* surface_data = noia_surface_get(sid);
    if (surface_data) {
        cursor_sid = sid;
    }

    pthread_mutex_unlock(&pointer_mutex);
}

//------------------------------------------------------------------------------

/// Handle surface destruction notification.
void noia_exhibitor_pointer_on_surface_destroyed(void* edata,
                                                 void* sdata NOIA_UNUSED)
{
    pthread_mutex_lock(&pointer_mutex);

    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    if (cursor_sid == sid) {
        cursor_sid = scInvalidSurfaceId;
    }

    pthread_mutex_unlock(&pointer_mutex);
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaPosition noia_exhibitor_pointer_get_global_position(void)
{
    return position;
}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_exhibitor_pointer_get_sid(void)
{
    return cursor_sid;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_update_hover_state(NoiaOutput* output,
                                               NoiaPool* visible_surfaces)
{
    if (output != active_output) {
        return;
    }

    pthread_mutex_lock(&pointer_mutex);

    NoiaPosition relative_position = {position.x - output->area.pos.x,
                                      position.y - output->area.pos.y};

    NoiaPosition relative = {-1, -1};
    NoiaSurfaceId sid = scInvalidSurfaceId;

    int i;
    NoiaSurfaceContext* context;
    NOIA_ITERATE_POOL_REVERSE(visible_surfaces, i, context) {
        NoiaSurfaceData* data = noia_surface_get(context->sid);
        /// @todo Use noia_position_is_inside function
        if (data
        &&  relative_position.x >= context->pos.x
        &&  relative_position.y >= context->pos.y
        &&  relative_position.x <= context->pos.x + data->desired_size.width
        &&  relative_position.y <= context->pos.y + data->desired_size.height) {
            sid = context->sid;
            relative.x = relative_position.x - context->pos.x + data->offset.x;
            relative.y = relative_position.y - context->pos.y + data->offset.y;
            break;
        }
    }

    if (sid != focused_sid) {
        LOG_INFO2("Pointer focus changed "
                  "(old sid: %d, new sid: %d, x: %d, y: %d)",
                  focused_sid, sid, relative.x, relative.y);
        focused_sid = sid;
        cursor_sid = scInvalidSurfaceId;
        noia_event_signal_emit(SIGNAL_POINTER_FOCUS_CHANGED,
                               (NoiaObject*) noia_motion_create(sid, relative));
    } else if (focused_sid != scInvalidSurfaceId
           && (relative.x != last_rel.x || relative.y != last_rel.y)) {
        LOG_INFO3("Pointer surface relative position (x: %d, y: %d)",
                  relative.x, relative.y);
        noia_event_signal_emit(SIGNAL_POINTER_RELATIVE_MOTION,
                               (NoiaObject*) noia_motion_create(sid, relative));
        last_rel = relative;
    }

    pthread_mutex_unlock(&pointer_mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_initialize(NoiaLoop* this_loop, void* data)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    focused_sid = scInvalidSurfaceId;

    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_RESET,
               noia_task_create(noia_exhibitor_pointer_on_motion_reset,
                                this_loop, data));

    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_X,
               noia_task_create(noia_exhibitor_pointer_on_motion_x,
                                this_loop, data));

    noia_event_signal_subscribe(SIGNAL_POINTER_MOTION_Y,
               noia_task_create(noia_exhibitor_pointer_on_motion_y,
                                this_loop, data));

    noia_event_signal_subscribe(SIGNAL_CURSOR_SURFACE_CHANGE,
               noia_task_create(noia_exhibitor_pointer_on_surface_change,
                                this_loop, data));

    noia_event_signal_subscribe(SIGNAL_SURFACE_DESTROYED,
               noia_task_create(noia_exhibitor_pointer_on_surface_destroyed,
                                this_loop, data));
}

//------------------------------------------------------------------------------

