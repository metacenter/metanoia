// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

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

