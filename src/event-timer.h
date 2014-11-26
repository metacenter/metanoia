// file: event-timer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_TIMER_H__
#define __AURA_EVENT_TIMER_H__

#include <sys/signal.h>

typedef struct AuraEventDispatcherPriv AuraEventDispatcher;

typedef void (*AuraTimerHandler) (union sigval data);

int aura_event_timer_run(AuraTimerHandler timer_handler,
                         int miliseconds);

#endif // __AURA_EVENT_TIMER_H__

