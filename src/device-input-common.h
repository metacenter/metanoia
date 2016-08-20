// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_INPUT_COMMON_H
#define NOIA_DEVICE_INPUT_COMMON_H

#include "global-enums.h"
#include "input-context.h"

/// Handle events coming from keyboard devices.
/// Check if currently pressed keys relate to any defined key binding.
/// If so, execute the binding, otherwise signal key press event.
void noia_input_handle_key(unsigned time,
                           unsigned code,
                           NoiaKeyState value,
                           NoiaInputContext* context);

/// Handle pointer motion events.
/// Scale event values according to configuration and signal event.
void noia_input_handle_pointer_motion(int x, int y);

/// Handle pointer position events.
/// Scale event values according to configuration and signal event.
void noia_input_handle_pointer_position(int x, int y);

/// Handle pointer button events.
void noia_input_handle_pointer_button(unsigned time, unsigned code, bool value);

/// Handle pointer axis (scroll) events.
/// Scale event values according to configuration and signal event.
void noia_input_handle_pointer_axis(double h, double v, int hd, int vd);

/// Handle pointer position reset.
void noia_input_handle_pointer_position_reset();

#endif // NOIA_DEVICE_INPUT_COMMON_H

