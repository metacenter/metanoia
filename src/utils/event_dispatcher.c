// file: event_dispatcher.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event_dispatcher.h"
#include "utils-log.h"

#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef struct {
    int epfd;
    bool run;
    long timeout;
} AuraEventDispatcherPriv;

//------------------------------------------------------------------------------

AuraEventDispatcher* aura_event_dispatcher_new()
{
    AuraEventDispatcherPriv* mine = malloc(sizeof(AuraEventDispatcherPriv));
    if (!mine) {
        return NULL;
    }

    mine->epfd = epoll_create1(0);
    mine->run = 0;

    return (AuraEventDispatcher*) mine;
}

//------------------------------------------------------------------------------

void aura_event_dispatcher_free(AuraEventDispatcher* self)
{
    AuraEventDispatcherPriv* mine = (AuraEventDispatcherPriv*) self;
    if (!mine) {
        return;
    }

    free(mine);
}

//------------------------------------------------------------------------------

int aura_event_dispatcher_add_event_source(AuraEventDispatcher* self,
                                           AuraEventData* data)
{
    AuraEventDispatcherPriv* mine = (AuraEventDispatcherPriv*) self;
    if (!mine || !data) {
        return -ENOMEM;
    }

    LOG_INFO2("Adding event source (fd: '%d', flags: '%d')",
              data->fd, data->flags);

    struct epoll_event event;
    event.data.ptr = data;
    event.events = EPOLLIN | EPOLLET;

    return epoll_ctl(mine->epfd, EPOLL_CTL_ADD, data->fd, &event);
}

//------------------------------------------------------------------------------

void aura_event_dispatcher_start(AuraEventDispatcher* self)
{
    int r;
    struct epoll_event event;

    AuraEventDispatcherPriv* mine = (AuraEventDispatcherPriv*) self;
    if (!mine) {
        return;
    }

    LOG_INFO1("Event Dispatcher: Started");

    mine->run = 1;
    while (mine->run) {
        LOG_INFO4("Waiting for events...");
        r = epoll_wait(mine->epfd, &event, 1, -1);

        if (r > 0) {
            AuraEventData* data = event.data.ptr;
            if (data) {
                LOG_INFO4("New event from %d", data->fd);
                if (data->handler) {
                    data->handler(data, event);
                }
            }
        } else if (r < 0) {
            LOG_ERROR("Epoll Error! (%d)", r);
            mine->run = 0;
        } else {
            LOG_WARN1("Ut infinitum et ultra!");
        }
    }

    LOG_INFO1("Event Dispatcher: Stoped");
}

//------------------------------------------------------------------------------

void aura_event_dispatcher_stop(AuraEventDispatcher* self)
{
    AuraEventDispatcherPriv* mine = (AuraEventDispatcherPriv*) self;
    if (!mine) {
        return;
    }

    LOG_INFO1("Event Dispatcher: Stoping...");
    mine->run = 0;
}

//------------------------------------------------------------------------------

int aura_event_dispatcher_timer_run(AuraEventDispatcher* self,
                                    AuraTimerHandler timer_handler,
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

    return result;
}

//------------------------------------------------------------------------------

