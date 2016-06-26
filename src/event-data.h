// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EVENT_DATA_H
#define NOIA_EVENT_DATA_H

#include <sys/epoll.h>

typedef struct NoiaEventDataPriv NoiaEventData;

typedef void (*NoiaEventHandler) (NoiaEventData*, struct epoll_event*);
typedef void (*NoiaEventExitHandler) (NoiaEventData*);

/// Event Data constructor.
NoiaEventData* noia_event_data_create(int fd,
                                      void* data,
                                      NoiaEventHandler handler,
                                      NoiaEventExitHandler exit);

/// Event data destructor.
/// @note Does not free stored data!
void noia_event_data_destroy(NoiaEventData* self);

/// Returns event source file descriptor.
int noia_event_data_get_fd(NoiaEventData* self);

/// Return user data.
void* noia_event_data_get_data(NoiaEventData* self);

/// Call event handler.
void noia_event_data_handle(NoiaEventData* self, struct epoll_event* epev);

/// Call dispatcher destruction handler.
void noia_event_data_exit(NoiaEventData* self);

#endif // NOIA_EVENT_DATA_H

