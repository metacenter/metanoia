// file: event-signals.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_SIGNALS_H__
#define __AURA_EVENT_SIGNALS_H__

#include "event-task.h"

typedef enum {
    SIGNAL_SCREEN_REFRESH = 0,
    SIGNAL_DISPLAY_DISCOVERED,
    SIGNAL_DISPLAY_FOUND,
    SIGNAL_DISPLAY_LOST,
    SIGNAL_SURFACE_CREATED,
    SIGNAL_SURFACE_DESTROYED,
    SIGNAL_KEYBOARD_FOCUS_CHANGED,
    SIGNAL_KEYBOARD_EVENT,
    SIGNAL_NUM,
} AuraSignalNum;

int aura_event_signal_subscribe(AuraSignalNum sig_num, AuraTask* task);

int aura_event_signal_emit(AuraSignalNum sig_num, void* data);

#endif // __AURA_EVENT_SIGNAL_H__

