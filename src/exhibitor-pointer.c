// file: exhibitor-pointer.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-pointer.h"
#include "exhibitor.h"
#include "surface-manager.h"

#include "utils-log.h"
#include "global-objects.h"
#include "event-signals.h"

#define INVALID_POINTER_VALUE -1

static NoiaSurfaceId cursor_sid = 0;
static NoiaSurfaceData* cursor_data = NULL;
static NoiaPosition position = {100, 100};
static NoiaPosition last_abs = {INVALID_POINTER_VALUE, INVALID_POINTER_VALUE};
static NoiaPosition last_rel = {INVALID_POINTER_VALUE, INVALID_POINTER_VALUE};
static NoiaSurfaceId focused_sid;

//------------------------------------------------------------------------------

NoiaPosition noia_exhibitor_pointer_get_position()
{
    return position;
}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_exhibitor_pointer_get_sid()
{
    return cursor_sid;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_invalidate_surface()
{
    cursor_sid = scInvalidSurfaceId;
    cursor_data = NULL;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_update_hover_state(NoiaList* visible_surfaces)
{
    NoiaPosition relative = {-1, -1};
    NoiaSurfaceId sid = scInvalidSurfaceId;
    FOR_EACH_REVERSE (visible_surfaces, link) {
        NoiaSurfaceId current_sid = (NoiaSurfaceId) link->data;
        NoiaSurfaceData* data = noia_surface_get(current_sid);
        if (data
        &&  position.x >= data->position.x
        &&  position.y >= data->position.y
        &&  position.x <= data->position.x + data->desired_size.width
        &&  position.y <= data->position.y + data->desired_size.height) {
            sid = current_sid;
            relative.x = position.x - data->position.x + data->offset.x;
            relative.y = position.y - data->position.y + data->offset.y;
            break;
        }
    }

    if (sid != focused_sid) {
        LOG_INFO2("Pointer focus changed "
                  "(old sid: %d, new sid: %d, x: %d, y: %d)",
                  focused_sid, sid, relative.x, relative.y);
        focused_sid = sid;
        noia_event_signal_emit(SIGNAL_POINTER_FOCUS_CHANGED,
                               (NoiaObject*) noia_motion_create(sid, relative));
        noia_exhibitor_pointer_invalidate_surface();
    } else if (focused_sid != scInvalidSurfaceId
           && (relative.x != last_rel.x || relative.y != last_rel.y)) {
        LOG_INFO3("Pointer surface relative position (x: %d, y: %d)",
                  relative.x, relative.y);
        noia_event_signal_emit(SIGNAL_POINTER_RELATIVE_MOTION,
                               (NoiaObject*) noia_motion_create(sid, relative));
        last_rel = relative;
    }

    if (cursor_data) {
        cursor_data->position = position;
    }
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_motion_reset()
{
    last_abs.x = INVALID_POINTER_VALUE;
    last_abs.y = INVALID_POINTER_VALUE;
}

//------------------------------------------------------------------------------

/// @todo Implement pointer borders
void noia_exhibitor_pointer_on_motion_x(void* data)
{
    int abs_value = noia_int_unref_get((NoiaIntObject*) data);
    if (last_abs.x != INVALID_POINTER_VALUE) {
        position.x += abs_value - last_abs.x;

//        NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
//        int max = exhibitor->display->output->width - 1;
        int max = 400;

        if (position.x < 0) {
            position.x = 0;
        }
        if (position.x > max) {
            position.x = max;
        }
    }
    last_abs.x = abs_value;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_motion_y(void* data)
{
    int abs_value = noia_int_unref_get((NoiaIntObject*) data);
    if (last_abs.y != INVALID_POINTER_VALUE) {
        position.y += abs_value - last_abs.y;

//        NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
//        int max = exhibitor->display->output->height - 1;
        int max = 400;

        if (position.y < 0) {
            position.y = 0;
        }
        if (position.y > max) {
            position.y = max;
        }
    }
    last_abs.y = abs_value;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_surface_change(void* data)
{
    NoiaSurfaceId sid = noia_int_unref_get((NoiaIntObject*) data);
    NoiaSurfaceData* surface_data = noia_surface_get(sid);
    if (!surface_data) {
        return;
    }

    cursor_data = surface_data;
    cursor_data->position = position;
    cursor_sid = sid;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_surface_destroyed(void* data)
{
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) data);
    if (sid != cursor_sid) {
        return;
    }
    noia_exhibitor_pointer_invalidate_surface();
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

