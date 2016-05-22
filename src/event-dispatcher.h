// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EVENT_DISPATCHER_H
#define NOIA_EVENT_DISPATCHER_H

#include <sys/epoll.h>

typedef struct NoiaEventData NoiaEventData;
typedef struct NoiaEventDispatcherPriv NoiaEventDispatcher;

typedef void (*NoiaEventHandler) (NoiaEventData*, struct epoll_event*);
typedef void (*NoiaEventExitHandler) (NoiaEventData*);

/// @todo Move NoiaEventData to separate file and change name.
struct NoiaEventData {
    int fd;
    NoiaEventHandler handler;
    NoiaEventExitHandler exit;
    uint32_t flags;
    void* data;
};

NoiaEventData* noia_event_data_create(int fd,
                                      NoiaEventHandler handler,
                                      NoiaEventExitHandler exit,
                                      uint32_t flags,
                                      void* data);

/// Does not free stored data!
void noia_event_data_destroy(NoiaEventData* event_data);

NoiaEventDispatcher* noia_event_dispatcher_new(void);
void noia_event_dispatcher_free(NoiaEventDispatcher* self);

int noia_event_dispatcher_is_running(NoiaEventDispatcher* self);

int noia_event_dispatcher_initialize(NoiaEventDispatcher* self);

int noia_event_dispatcher_add_event_source(NoiaEventDispatcher* self,
                                           NoiaEventData* data);

void noia_event_dispatcher_start(NoiaEventDispatcher* self);
void noia_event_dispatcher_stop(NoiaEventDispatcher* self);

void noia_event_dispatcher_default_signal_handler(NoiaEventData* data,
                                                  struct epoll_event* event);

#endif // NOIA_EVENT_DISPATCHER_H

