// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "event-timer.h"
#include "global-macros.h"
#include "utils-log.h"

#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/timerfd.h>

//------------------------------------------------------------------------------

struct NoiaTimerPriv {
    bool is_running;
    void* data;
    NoiaEventHandler handler;
    int fd;
};

//------------------------------------------------------------------------------

NoiaTimer* noia_timer_create(void* data, NoiaEventHandler handler)
{
    NoiaTimer* self = malloc(sizeof(*self));
    self->is_running = false;
    self->data = data;
    self->handler = handler;
    self->fd = timerfd_create(CLOCK_MONOTONIC, 0x0);
    return self;
}

//------------------------------------------------------------------------------

void noia_timer_destroy(NoiaTimer* self)
{
    NOIA_ENSURE(self, return);
    close(self->fd);
    free(self);
}

//------------------------------------------------------------------------------

void noia_timer_run(NoiaTimer* self, NoiaMilliseconds milliseconds)
{
    struct itimerspec its;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = milliseconds / 1000;
    its.it_value.tv_nsec = (milliseconds % 1000) * 1000;

    if (timerfd_settime(self->fd, 0x0, &its, NULL) == 0) {
        self->is_running = true;
    } else {
        LOG_ERROR("Error starting timer: %m");
    }
}

//------------------------------------------------------------------------------

void noia_timer_cancel(NoiaTimer* self)
{
    struct itimerspec its;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;

    if (timerfd_settime(self->fd, 0x0, &its, NULL) == 0) {
        self->is_running = false;
    } else {
        LOG_ERROR("Error stopping timer: %m");
    }
}

//------------------------------------------------------------------------------

void* noia_timer_get_data(NoiaTimer* self)
{
    return self->data;
}

//------------------------------------------------------------------------------

NoiaEventData* noia_timer_create_event_data(NoiaTimer* self)
{
    return noia_event_data_create(self->fd, self->data, self->handler, NULL);
}

//------------------------------------------------------------------------------

