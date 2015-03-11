// file: event-task.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-task.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

static NoiaTask sDummyTask = {{0, NULL}, NULL, NULL, NULL, NULL};

//------------------------------------------------------------------------------

NoiaTask* noia_task_new(NoiaTaskProcessor process,
                        NoiaTaskFreeFunc free,
                        NoiaLoop* loop,
                        void* subscription_data,
                        void* emission_data)
{
    NoiaTask* self = malloc(sizeof(NoiaTask));
    if (!self) {
        return &sDummyTask;
    }

    noia_object_initialize(&self->base, free);
    noia_object_ref(&self->base);

    self->process = process;
    self->loop = loop;
    self->subscription_data = subscription_data;
    self->emission_data = emission_data;
    return self;
}

//------------------------------------------------------------------------------

NoiaTask* noia_task_create(NoiaTaskProcessor process,
                           NoiaLoop* loop,
                           void* subscription_data)
{
    return noia_task_new(process, (NoiaTaskFreeFunc) noia_task_free,
                         loop, subscription_data, NULL);
}

//------------------------------------------------------------------------------

NoiaTask* noia_task_copy(NoiaTask* task)
{
    return noia_task_new(task->process, task->base.free, task->loop,
                         task->subscription_data, task->emission_data);
}

//------------------------------------------------------------------------------

/// Free only task, keep data.
void noia_task_free(NoiaTask* self)
{
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(NoiaTask));
    free(self);
}

//------------------------------------------------------------------------------

