// file: event-timer.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-timer.h"
#include "utils-log.h"

#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

//------------------------------------------------------------------------------

timer_t aura_event_timer_run(int miliseconds,
                             AuraTimerHandler timer_handler,
                             void* data)
{
    timer_t timerid;
    struct sigevent se;
    struct itimerspec its;

    memset(&se, 0, sizeof(se));

    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_attributes = NULL;
    se.sigev_notify_function = timer_handler;
    se.sigev_value.sival_ptr = data;

    if (timer_create(CLOCK_MONOTONIC, &se, &timerid) < 0) {
        LOG_ERROR("Could not create timer: %s", strerror(errno));
        return 0;
    }

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;

    if (miliseconds > 999) {
        its.it_value.tv_sec = miliseconds / 1000;
        its.it_interval.tv_sec = miliseconds / 1000;
    } else {
        its.it_value.tv_nsec = 1000000 * miliseconds;
        its.it_interval.tv_nsec = 1000000 * miliseconds;
    }

    if (timer_settime(timerid, 0, &its, NULL) < 0) {
        LOG_ERROR("Could not start timer: %s", strerror(errno));
        return 0;
    }

    LOG_INFO2("Timer started (ms: %d)", miliseconds);

    return timerid;
}

//------------------------------------------------------------------------------

void aura_event_timer_delete(timer_t timerid)
{
    timer_delete(timerid);
}

//------------------------------------------------------------------------------

void aura_event_timer_nanosleep(long nanoseconds)
{
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = nanoseconds;

    if(nanosleep(&req, NULL)) {
        LOG_ERROR("Sleep error: %m");
    }
}

//------------------------------------------------------------------------------

