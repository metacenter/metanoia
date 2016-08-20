// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-input-common.h"
#include "event-signals.h"
#include "config.h"
#include "input-bindings.h"

#include <linux/input.h>

//------------------------------------------------------------------------------

void noia_input_handle_key(unsigned time,
                           unsigned code,
                           NoiaKeyState value,
                           NoiaInputContext* context)
{
    NOIA_BLOCK {
        // Ignore repeated mask key
        if (value) {
            if ((((code == KEY_LEFTCTRL) or (code == KEY_RIGHTCTRL))
                and (context->modifiers & NOIA_KEY_CTRL))
             or (((code == KEY_LEFTSHIFT) or (code == KEY_RIGHTSHIFT))
                and (context->modifiers & NOIA_KEY_SHIFT))
             or (((code == KEY_LEFTALT) or (code == KEY_RIGHTALT))
                and (context->modifiers & NOIA_KEY_ALT))
             or (((code == KEY_LEFTMETA) or (code == KEY_RIGHTMETA))
                and (context->modifiers & NOIA_KEY_META))) {
                break;
            }
        }

        // Check for key bindings
        bool catched = noia_input_catch_key(noia_gears()->modes,
                                            context, code, value);
        if (catched) {
            break;
        }

        // If no binding found inform the rest of the world
        NoiaKeyObject* key = noia_key_create(time, code, value);
        noia_event_signal_emit(SIGNAL_INPUT_KEYBOARD, &key->base);
        noia_object_unref(&key->base);
    }
}

//------------------------------------------------------------------------------

void noia_input_handle_pointer_motion(int x, int y)
{
    // Scale event values
    NoiaPosition vector = {
            .x = x * noia_config()->mouse_scale,
            .y = y * noia_config()->mouse_scale,
        };
    // Signal event
    NoiaMotionObject* motion = noia_motion_create(scInvalidSurfaceId, vector);
    noia_event_signal_emit(SIGNAL_INPUT_POINTER_MOTION, &motion->base);
    noia_object_unref(&motion->base);
}

//------------------------------------------------------------------------------

void noia_input_handle_pointer_position(int x, int y)
{
    // Scale event values. Skip scaling invalid values
    double s = noia_config()->touchpad_scale;
    NoiaPosition pos = {
            .x = (x > 0) ? s*x : x,
            .y = (y > 0) ? s*y : y,
        };
    // Signal event
    NoiaMotionObject* position = noia_motion_create(scInvalidSurfaceId, pos);
    noia_event_signal_emit(SIGNAL_INPUT_POINTER_POSITION, &position->base);
    noia_object_unref(&position->base);
}
//------------------------------------------------------------------------------

void noia_input_handle_pointer_button(unsigned time, unsigned code, bool value)
{
    NoiaButtonObject* btn = noia_button_create(time, code, value);
    noia_event_signal_emit(SIGNAL_INPUT_POINTER_BUTTON, &btn->base);
    noia_object_unref(&btn->base);
}

//------------------------------------------------------------------------------

void noia_input_handle_pointer_axis(double h, double v, int hd, int vd)
{
    double s = noia_config()->axis_scale;
    NoiaAxisObject* axis = noia_axis_create(s*h, s*v, hd, vd);
    noia_event_signal_emit(SIGNAL_INPUT_POINTER_AXIS, &axis->base);
    noia_object_unref(&axis->base);
}

//------------------------------------------------------------------------------

void noia_input_handle_pointer_position_reset()
{
    noia_event_signal_emit(SIGNAL_INPUT_POINTER_POSITION_RESET, NULL);
}

//------------------------------------------------------------------------------

