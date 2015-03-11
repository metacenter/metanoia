// file: event-task.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EVENT_TASK_H__
#define __NOIA_EVENT_TASK_H__

#include "utils-object.h"

typedef struct NoiaLoopPriv NoiaLoop;

typedef void (*NoiaTaskProcessor) (void*);
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

#endif // __NOIA_EVENT_TASK_H__

