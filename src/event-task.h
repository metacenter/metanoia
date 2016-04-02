// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EVENT_TASK_H
#define NOIA_EVENT_TASK_H

#include "utils-object.h"

typedef struct NoiaLoopPriv NoiaLoop;

typedef void (*NoiaTaskProcessor) (void*, void*);
typedef void (*NoiaTaskFreeFunc) (void*);

typedef struct {
    NoiaObject base;
    NoiaTaskProcessor process;
    NoiaLoop* loop;
    void* subscription_data;
    void* emission_data;
} NoiaTask;

NoiaTask* noia_task_new(NoiaTaskProcessor process,
                        NoiaTaskFreeFunc free,
                        NoiaLoop* loop,
                        void* subscription_data,
                        void* emission_data);

NoiaTask* noia_task_create(NoiaTaskProcessor process,
                           NoiaLoop* loop,
                           void* subscription_data);

NoiaTask* noia_task_copy(NoiaTask* task);

void noia_task_free(NoiaTask *self);

#endif // NOIA_EVENT_TASK_H

