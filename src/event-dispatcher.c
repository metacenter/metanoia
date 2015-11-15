// file: event-dispatcher.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-dispatcher.h"
#include "utils-log.h"
#include "utils-list.h"
#include "global-macros.h"

#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <signal.h>
#include <unistd.h>
#include <sys/signalfd.h>

struct NoiaEventDispatcherPriv {
    int run;
    int epfd;
    NoiaList* sources;
};

//------------------------------------------------------------------------------

NoiaEventData* noia_event_data_create(int fd,
                                      NoiaEventHandler handler,
                                      NoiaEventExitHandler exit,
                                      uint32_t flags,
                                      void* data)
{
    NoiaEventData* self = malloc(sizeof(NoiaEventData));
    if (!self) {
        return NULL;
    }

    self->fd = fd;
    self->handler = handler;
    self->exit = exit;
    self->flags = flags;
    self->data = data;
    return self;
}

//------------------------------------------------------------------------------

void noia_event_data_destroy(NoiaEventData* event_data)
{
    if (!event_data) {
        return;
    }

    memset(event_data, 0, sizeof(NoiaEventData));
    free(event_data);
}

//------------------------------------------------------------------------------

NoiaEventDispatcher* noia_event_dispatcher_new()
{
    NoiaEventDispatcher* self = malloc(sizeof(NoiaEventDispatcher));
    if (!self) {
        return NULL;
    }

    self->run = 0;
    self->sources = noia_list_new((NoiaFreeFunc) noia_event_data_destroy);
    return self;
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_free(NoiaEventDispatcher* self)
{
    if (!self) {
        return;
    }

    noia_list_free(self->sources);
    memset(self, 0, sizeof(NoiaEventDispatcher));
    free(self);
}

//------------------------------------------------------------------------------

int noia_event_dispatcher_is_running(NoiaEventDispatcher* self)
{
    if (!self) {
        return 0;
    }

    return self->run;
}

//------------------------------------------------------------------------------

int noia_event_dispatcher_initialize(NoiaEventDispatcher* self)
{
    if (!self) {
        return -1;
    }

    // Create epoll
    self->epfd = epoll_create1(0);
    if (self->epfd == -1) {
        LOG_ERROR("Failed to create epoll FD!");
        return -1;
    }

    return 0;
}

//------------------------------------------------------------------------------

int noia_event_dispatcher_add_event_source(NoiaEventDispatcher* self,
                                           NoiaEventData* data)
{
    if (!self || !data) {
        return -ENOMEM;
    }

    LOG_INFO2("Adding event source (fd: '%d')", data->fd);

    noia_list_append(self->sources, data);

    struct epoll_event event;
    event.data.ptr = data;
    event.events = EPOLLIN;

    return epoll_ctl(self->epfd, EPOLL_CTL_ADD, data->fd, &event);
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_start(NoiaEventDispatcher* self)
{
    if (!self) {
        return;
    }

    LOG_INFO1("Event Dispatcher: Started");

    self->run = 1;
    while (self->run) {
        LOG_EVNT4("Waiting for events...");
        struct epoll_event event;
        int r = epoll_wait(self->epfd, &event, 1, -1);
        if (r > 0) {
            NoiaEventData* data = event.data.ptr;
            if (data) {
                LOG_EVNT4("New event from %d", data->fd);
                if (data->handler) {
                    data->handler(data, &event);
                }
            }
        } else if (r < 0) {
            if (errno != EINTR) {
                LOG_ERROR("Epoll Error! (%d, %m)", errno);
                self->run = 0;
            }
        } else {
            LOG_WARN1("Ut infinitum et ultra!");
        }
    }

    FOR_EACH (self->sources, link) {
        NoiaEventData* data = link->data;
        if (data && data->exit) {
            data->exit(data);
        }
    }

    LOG_INFO1("Event Dispatcher: Stoped");
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_stop(NoiaEventDispatcher* self)
{
    if (!self) {
        return;
    }

    LOG_INFO1("Event Dispatcher: Stoping...");
    self->run = 0;
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_default_signal_handler
                                         (NoiaEventData* data,
                                          struct epoll_event* event NOIA_UNUSED)
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
        NoiaEventDispatcher* dispather = data->data;
        if (dispather && noia_event_dispatcher_is_running(dispather)) {
            noia_event_dispatcher_stop(dispather);
        } else {
            LOG_ERROR("Invalid dispatcher!");
        }
    } else {
        LOG_INFO2("Unhandled signal: '%d'", fdsi.ssi_signo);
    }
}

//------------------------------------------------------------------------------

