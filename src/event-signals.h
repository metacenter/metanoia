// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EVENT_SIGNALS_H
#define NOIA_EVENT_SIGNALS_H

#include "event-task.h"

typedef enum {
    SIGNAL_SCREEN_REFRESH = 0,
    SIGNAL_ACTION,
    SIGNAL_OUTPUTS_CHANGED,
    SIGNAL_OUTPUT_FOUND,
    SIGNAL_OUTPUT_LOST,
    SIGNAL_SURFACE_READY,
    SIGNAL_SURFACE_DESTROYED,
    SIGNAL_SURFACE_RECONFIGURED,
    SIGNAL_CURSOR_SURFACE_CHANGE,
    SIGNAL_INPUT_KEYBOARD,
    SIGNAL_INPUT_POINTER_MOTION,
    SIGNAL_INPUT_POINTER_POSITION,
    SIGNAL_INPUT_POINTER_BUTTON,
    SIGNAL_INPUT_POINTER_AXIS,
    SIGNAL_INPUT_POINTER_POSITION_RESET,
    SIGNAL_KEYBOARD_FOCUS_CHANGED,
    SIGNAL_POINTER_FOCUS_CHANGED,
    SIGNAL_POINTER_RELATIVE_MOTION,
    SIGNAL_NOTIFY,
    SIGNAL_NUM,
} NoiaSignalNum;

NoiaResult noia_event_signal_subscribe(NoiaSignalNum sig_num, NoiaTask* task);

NoiaResult noia_event_signal_unsubscribe(void* subscription_data);

NoiaResult noia_event_signal_emit(NoiaSignalNum sig_num, NoiaObject* object);

NoiaResult noia_event_signal_emit_int(NoiaSignalNum sig_num, intptr_t value);

void noia_event_signal_clear_all_substriptions(void);

#endif // NOIA_EVENT_SIGNAL_H

