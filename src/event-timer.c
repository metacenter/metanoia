// file: event-timer.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-timer.h"
#include "utils-log.h"

#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

//------------------------------------------------------------------------------

int aura_event_timer_run(AuraTimerHandler timer_handler,
                         int miliseconds)
{
    int result;
    timer_t timerid;
    struct sigevent se;
    struct itimerspec its;

    memset(&se, 0, sizeof(se));

    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_attributes = NULL;
    se.sigev_notify_function = timer_handler;

    if (timer_create(CLOCK_MONOTONIC, &se, &timerid) < 0) {
        LOG_ERROR("Could not create timer: %s", strerror(errno));
        return -1;
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

    result = timer_settime(timerid, 0, &its, NULL);
    if (result < 0) {
        LOG_ERROR("Could not start timer: %s", strerror(errno));
        return -1;
    }

    LOG_INFO2("Timer started (ms: %d)", miliseconds);

    return result;
}

//------------------------------------------------------------------------------

