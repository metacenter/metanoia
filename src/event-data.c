// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "event-data.h"

#include "global-macros.h"

#include <memory.h>

//------------------------------------------------------------------------------

struct NoiaEventDataPriv {
    int fd;
    void* data;
    NoiaEventHandler handler;
    NoiaEventExitHandler exit;
};

//------------------------------------------------------------------------------

NoiaEventData* noia_event_data_create(int fd,
                                      void* data,
                                      NoiaEventHandler handler,
                                      NoiaEventExitHandler exit)
{
    NoiaEventData* self = malloc(sizeof(*self));
    self->fd = fd;
    self->data = data;
    self->handler = handler;
    self->exit = exit;
    return self;
}

//------------------------------------------------------------------------------

void noia_event_data_destroy(NoiaEventData* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(*self));
    free(self);
}

//------------------------------------------------------------------------------

int noia_event_data_get_fd(NoiaEventData* self)
{
    NOIA_ENSURE(self, return -1);
    return self->fd;
}

//------------------------------------------------------------------------------

void* noia_event_data_get_data(NoiaEventData* self)
{
    NOIA_ENSURE(self, return NULL);
    return self->data;
}

//------------------------------------------------------------------------------

void noia_event_data_handle(NoiaEventData* self, struct epoll_event* epev)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->handler, return);
    self->handler(self, epev);
}

//------------------------------------------------------------------------------

void noia_event_data_exit(NoiaEventData* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->exit, return);
    self->exit(self);
}

//------------------------------------------------------------------------------

