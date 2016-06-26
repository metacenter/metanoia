// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EVENT_TIMER_H
#define NOIA_EVENT_TIMER_H

#include "global-types.h"
#include "event-data.h"

/// Timer is used to perform delayed action using `NoiaEventDispatcher`.
typedef struct NoiaTimerPriv NoiaTimer;

typedef void (*NoiaTimerHandler) (NoiaTimer*, void*);

/// Timer constructor.
NoiaTimer* noia_timer_create(void* data, NoiaEventHandler handler);

/// Timer destructor.
void noia_timer_destroy(NoiaTimer* self);

/// Runs/starts/arms timer to be fired after given number of milliseconds.
void noia_timer_run(NoiaTimer* self, NoiaMilliseconds milliseconds);

/// Cancels/stops/disarms timer.
void noia_timer_cancel(NoiaTimer* self);

/// Return associated user data.
void* noia_timer_get_data(NoiaTimer* self);

/// Create event data that can be used as event source for Dispatcher.
/// @see noia_event_dispatcher_add_event_source
NoiaEventData* noia_timer_create_event_data(NoiaTimer* self);

#endif // NOIA_EVENT_TIMER_H

