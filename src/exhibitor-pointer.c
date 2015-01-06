// file: exhibitor-pointer.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-pointer.h"
#include "exhibitor.h"

#include "utils-log.h"
#include "event-signals.h"

#define INVALID_POINTER_VALUE -1

static AuraPosition position = {100, 100};
static int last_abs_value_x = INVALID_POINTER_VALUE;
static int last_abs_value_y = INVALID_POINTER_VALUE;

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_on_motion_reset()
{
    last_abs_value_x = INVALID_POINTER_VALUE;
    last_abs_value_y = INVALID_POINTER_VALUE;
}

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_on_motion_x(void* data)
{
    int abs_value = (int) data;

    if (last_abs_value_x != INVALID_POINTER_VALUE) {
        position.x += abs_value - last_abs_value_x;

        AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
        int max = exhibitor->display->output->width;

        if (position.x < 0) {
            position.x = 0;
        }
        if (position.x > max) {
            position.x = max - 1;
        }
    }
    last_abs_value_x = abs_value;
}

//------------------------------------------------------------------------------

void aura_exhibitor_pointer_on_motion_y(void* data)
{
    int abs_value = (int) data;

    if (last_abs_value_y != INVALID_POINTER_VALUE) {
        position.y += abs_value - last_abs_value_y;

        AuraExhibitor* exhibitor = aura_exhibitor_get_instance();
        int max = exhibitor->display->output->height;

        if (position.y < 0) {
            position.y = 0;
        }
        if (position.y > max) {
            position.y = max - 1;
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

void aura_exhibitor_pointer_initialize(AuraLoop* this_loop)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    aura_event_signal_subscribe(SIGNAL_POINTER_MOTION_RESET,
         aura_task_create(aura_exhibitor_pointer_on_motion_reset, this_loop));

    aura_event_signal_subscribe(SIGNAL_POINTER_MOTION_X,
         aura_task_create(aura_exhibitor_pointer_on_motion_x, this_loop));

    aura_event_signal_subscribe(SIGNAL_POINTER_MOTION_Y,
         aura_task_create(aura_exhibitor_pointer_on_motion_y, this_loop));
}

//------------------------------------------------------------------------------

