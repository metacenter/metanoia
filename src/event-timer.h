// file: event-timer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EVENT_TIMER_H
#define NOIA_EVENT_TIMER_H

#include <sys/signal.h>
#include <time.h>

typedef void (*NoiaTimerHandler) (union sigval data);

timer_t noia_event_timer_run(int miliseconds,
                             NoiaTimerHandler timer_handler,
                             void* data);

void noia_event_timer_delete(timer_t timerid);

void noia_event_timer_nanosleep(long nonoseconds);

#endif // NOIA_EVENT_TIMER_H

