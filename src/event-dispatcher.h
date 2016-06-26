// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EVENT_DISPATCHER_H
#define NOIA_EVENT_DISPATCHER_H

#include "global-types.h"
#include "event-data.h"

/// Event Dispatcher is main program loop.
/// It uses file descriptor mechanisms to wait for events and execute functions
/// assigned to them using `NoiaEventData`.
typedef struct NoiaEventDispatcherPriv NoiaEventDispatcher;

/// Dispatcher constructor.
NoiaEventDispatcher* noia_event_dispatcher_new(void);

/// Dispatcher destructor.
void noia_event_dispatcher_free(NoiaEventDispatcher* self);

/// Check if dispatcher loop is running.
bool noia_event_dispatcher_is_running(NoiaEventDispatcher* self);

/// Initialize dispatcher.
NoiaResult noia_event_dispatcher_initialize(NoiaEventDispatcher* self);

/// Add event source.
NoiaResult noia_event_dispatcher_add_event_source(NoiaEventDispatcher* self,
                                                  NoiaEventData* data);

/// Start dispatching loop.
void noia_event_dispatcher_start(NoiaEventDispatcher* self);

/// Stop dispatching loop.
void noia_event_dispatcher_stop(NoiaEventDispatcher* self);

/// Default implementation of signal handling:
/// on receiving `SIGINT` or `SIGTERM` stop dispatcher.
/// @see factorize_signal_event_data, noia_environment_async_signal_handler
void noia_event_dispatcher_default_signal_handler(NoiaEventData* data,
                                                  struct epoll_event* event);

#endif // NOIA_EVENT_DISPATCHER_H

