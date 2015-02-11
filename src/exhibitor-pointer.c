// file: exhibitor-pointer.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-pointer.h"
#include "exhibitor.h"
#include "surface-manager.h"

#include "utils-log.h"
#include "global-objects.h"
#include "event-signals.h"

#define INVALID_POINTER_VALUE -1

static AuraPosition position = {100, 100};
static int last_abs_value_x = INVALID_POINTER_VALUE;
static int last_abs_value_y = INVALID_POINTER_VALUE;
static AuraSurfaceId focused_sid;

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_on_motion_reset()
{
    last_abs_value_x = INVALID_POINTER_VALUE;
    last_abs_value_y = INVALID_POINTER_VALUE;
}

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_update_hover_state(AuraList* visible_surfaces)
{
    AuraSurfaceId sid = scInvalidSurfaceId;
    FOR_EACH_REVERSE (visible_surfaces, link) {
        AuraSurfaceId current_sid = (AuraSurfaceId) link->data;
        AuraSurfaceData* data = aura_surface_get(current_sid);
        if (data
        &&  position.x >= data->position.x
        &&  position.y >= data->position.y
        &&  position.x <= data->position.x + data->desired_size.width
        &&  position.y <= data->position.y + data->desired_size.height) {
            sid = current_sid;
            break;
        }
    }

    if (sid != focused_sid) {
        LOG_INFO3("Pointer focus changed from %d to %d", focused_sid, sid);
        focused_sid = sid;
        aura_event_signal_emit_int(SIGNAL_POINTER_FOCUS_CHANGED, sid);
    }
}

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_on_motion_x(void* data)
{
    int abs_value = aura_int_unref_get((AuraIntObject*) data);
    if (last_abs_value_x != INVALID_POINTER_VALUE) {
        position.x += abs_value - last_abs_value_x;

        AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
        int max = exhibitor->display->output->width - 1;

        if (position.x < 0) {
            position.x = 0;
        }
        if (position.x > max) {
            position.x = max;
        }
    }
    last_abs_value_x = abs_value;
}

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_on_motion_y(void* data)
{
    int abs_value = aura_int_unref_get((AuraIntObject*) data);
    if (last_abs_value_y != INVALID_POINTER_VALUE) {
        position.y += abs_value - last_abs_value_y;

        AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
        int max = exhibitor->display->output->height - 1;

        if (position.y < 0) {
            position.y = 0;
        }
        if (position.y > max) {
            position.y = max;
        }
    }
    last_abs_value_y = abs_value;
}

//------------------------------------------------------------------------------

AuraPosition aura_exhibitor_get_pointer_position()
{
    return position;
}

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_initialize(AuraLoop* this_loop, void* data)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    focused_sid = scInvalidSurfaceId;

    aura_event_signal_subscribe(SIGNAL_POINTER_MOTION_RESET,
               aura_task_create(aura_exhibitor_pointer_on_motion_reset,
                                this_loop, data));

    aura_event_signal_subscribe(SIGNAL_POINTER_MOTION_X,
               aura_task_create(aura_exhibitor_pointer_on_motion_x,
                                this_loop, data));

    aura_event_signal_subscribe(SIGNAL_POINTER_MOTION_Y,
               aura_task_create(aura_exhibitor_pointer_on_motion_y,
                                this_loop, data));
}

//------------------------------------------------------------------------------

