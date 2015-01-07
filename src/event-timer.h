// file: event-timer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_TIMER_H__
#define __AURA_EVENT_TIMER_H__

#include <sys/signal.h>
#include <time.h>

typedef void (*AuraTimerHandler) (union sigval data);

timer_t aura_event_timer_run(int miliseconds,
                             AuraTimerHandler timer_handler,
                             void* data);

void aura_event_timer_delete(timer_t timerid);

#endif // __AURA_EVENT_TIMER_H__

