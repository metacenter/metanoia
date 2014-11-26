// file: event-dispatcher.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-dispatcher.h"
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

