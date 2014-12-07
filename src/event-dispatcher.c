// file: event-dispatcher.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-dispatcher.h"
#include "utils-log.h"

#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <signal.h>
#include <unistd.h>
#include <sys/signalfd.h>

typedef struct {
    int run;
    int epfd;
} AuraEventDispatcherPriv;

//------------------------------------------------------------------------------

AuraEventDispatcher* aura_event_dispatcher_new()
{
    AuraEventDispatcherPriv* mine = malloc(sizeof(AuraEventDispatcherPriv));
    if (!mine) {
        return NULL;
    }

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

int aura_event_dispatcher_is_running(AuraEventDispatcher* self)
{
    AuraEventDispatcherPriv* mine = (AuraEventDispatcherPriv*) self;
    if (!mine) {
        return 0;
    }

    return mine->run;
}

//------------------------------------------------------------------------------

int aura_event_dispatcher_initialize(AuraEventDispatcher* self)
{
    AuraEventDispatcherPriv* mine = (AuraEventDispatcherPriv*) self;
    if (!mine) {
        return -1;
    }

    // Create epoll
    mine->epfd = epoll_create1(0);
    if (mine->epfd == -1) {
        LOG_ERROR("Failed to create epoll FD!");
        return -1;
    }

    return 0;
}

//------------------------------------------------------------------------------

int aura_event_dispatcher_add_event_source(AuraEventDispatcher* self,
                                           AuraEventData* data)
{
    AuraEventDispatcherPriv* mine = (AuraEventDispatcherPriv*) self;
    if (!mine || !data) {
        return -ENOMEM;
    }

    LOG_INFO2("Adding event source (fd: '%d')", data->fd);

    struct epoll_event event;
    event.data.ptr = data;
    event.events = EPOLLIN;

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
        LOG_EVNT4("Waiting for events...");
        r = epoll_wait(mine->epfd, &event, 1, -1);
        if (r > 0) {
            AuraEventData* data = event.data.ptr;
            if (data) {
                LOG_EVNT4("New event from %d", data->fd);
                if (data->handler) {
                    data->handler(data, event);
                }
            }
        } else if (r < 0) {
            LOG_ERROR("Epoll Error! (%d, %m)", r);
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

void aura_event_dispatcher_default_signal_handler(AuraEventData* data,
                                                  struct epoll_event event)
{
    int size;
    struct signalfd_siginfo fdsi;

    size = read(data->fd, &fdsi, sizeof(struct signalfd_siginfo));
    if (size != sizeof(struct signalfd_siginfo)) {
        if (size != -1) {
            LOG_ERROR("Epoll error!");
        }
        exit(-1);
    }

    LOG_INFO1("Signal '%d' received", fdsi.ssi_signo);

    if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGTERM) {
        AuraEventDispatcher* dispather = data->data.ptr;
        if (dispather && aura_event_dispatcher_is_running(dispather)) {
            aura_event_dispatcher_stop(dispather);
        } else {
            LOG_ERROR("Invalid dispatcher!");
        }
    } else {
        LOG_INFO2("Unhandled signal: '%d'", fdsi.ssi_signo);
    }
}

//------------------------------------------------------------------------------

