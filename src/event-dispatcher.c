// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

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

//------------------------------------------------------------------------------

struct NoiaEventDispatcherPriv {
    bool run;
    int epfd;
    NoiaList* sources;
};

//------------------------------------------------------------------------------

NoiaEventDispatcher* noia_event_dispatcher_new(void)
{
    NoiaEventDispatcher* self = malloc(sizeof(NoiaEventDispatcher));
    self->run = false;
    self->sources = noia_list_new((NoiaFreeFunc) noia_event_data_destroy);
    return self;
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_free(NoiaEventDispatcher* self)
{
    NOIA_ENSURE(self, return);
    noia_list_free(self->sources);
    memset(self, 0, sizeof(NoiaEventDispatcher));
    free(self);
}

//------------------------------------------------------------------------------

bool noia_event_dispatcher_is_running(NoiaEventDispatcher* self)
{
    NOIA_ENSURE(self, return false);
    return self->run;
}

//------------------------------------------------------------------------------

NoiaResult noia_event_dispatcher_initialize(NoiaEventDispatcher* self)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);

    NoiaResult result = NOIA_RESULT_ERROR;
    self->epfd = epoll_create1(0x0);
    if (self->epfd > 0) {
        result = NOIA_RESULT_SUCCESS;
    } else {
        LOG_ERROR("Failed to create epoll FD!");
    }

    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_event_dispatcher_add_event_source(NoiaEventDispatcher* self,
                                                  NoiaEventData* data)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(data, return NOIA_RESULT_INCORRECT_ARGUMENT);

    int fd = noia_event_data_get_fd(data);

    LOG_INFO2("Adding event source (fd: '%d')", fd);

    noia_list_append(self->sources, data);

    struct epoll_event event;
    event.data.ptr = data;
    event.events = EPOLLIN;

    NoiaResult result = NOIA_RESULT_ERROR;
    if (epoll_ctl(self->epfd, EPOLL_CTL_ADD, fd, &event) == 0) {
        result = NOIA_RESULT_SUCCESS;
    }
    return result;
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_start(NoiaEventDispatcher* self)
{
    NOIA_ENSURE(self, return);

    LOG_INFO1("Event Dispatcher: Started");

    self->run = true;
    while (self->run) {
        LOG_EVNT4("Waiting for events...");
        struct epoll_event event;
        int r = epoll_wait(self->epfd, &event, 1, -1);
        if (r > 0) {
            NoiaEventData* data = event.data.ptr;
            noia_event_data_handle(data, &event);
        } else if (r < 0) {
            if (errno != EINTR) {
                LOG_ERROR("Epoll Error! (%d, %m)", errno);
                self->run = false;
            }
        } else {
            LOG_WARN1("Ut infinitum et ultra!");
        }
    }

    FOR_EACH (self->sources, link) {
        NoiaEventData* data = link->data;
        noia_event_data_exit(data);
    }

    LOG_INFO1("Event Dispatcher: Stoped");
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_stop(NoiaEventDispatcher* self)
{
    NOIA_ENSURE(self, return);
    LOG_INFO1("Event Dispatcher: Stoping...");
    self->run = false;
}

//------------------------------------------------------------------------------

void noia_event_dispatcher_default_signal_handler
                                         (NoiaEventData* data,
                                          struct epoll_event* event NOIA_UNUSED)
{
    int fd = noia_event_data_get_fd(data);

    struct signalfd_siginfo fdsi;
    ssize_t size = read(fd, &fdsi, sizeof(struct signalfd_siginfo));
    if (size != sizeof(struct signalfd_siginfo)) {
        if (size != -1) {
            LOG_ERROR("Epoll error!");
        }
        abort();
    }

    LOG_INFO1("Signal '%d' received", fdsi.ssi_signo);

    if ((fdsi.ssi_signo == SIGINT) or (fdsi.ssi_signo == SIGTERM)) {
        NoiaEventDispatcher* dispather = noia_event_data_get_data(data);
        if (dispather and noia_event_dispatcher_is_running(dispather)) {
            noia_event_dispatcher_stop(dispather);
        } else {
            LOG_ERROR("Invalid dispatcher!");
        }
    } else {
        LOG_INFO2("Unhandled signal: '%d'", fdsi.ssi_signo);
    }
}

//------------------------------------------------------------------------------

