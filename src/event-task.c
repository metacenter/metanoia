// file: event-task.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-task.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

static AuraTask sDummyTask = {{0, NULL}, NULL, NULL, NULL, NULL};

//------------------------------------------------------------------------------

AuraTask* aura_task_new(AuraTaskProcessor process,
                        AuraTaskFreeFunc free,
                        AuraLoop* loop,
                        void* subscription_data,
                        void* emission_data)
{
    AuraTask* self = malloc(sizeof(AuraTask));
    if (!self) {
        return &sDummyTask;
    }

    aura_object_initialize(&self->base, free);
    aura_object_ref(&self->base);

    self->process = process;
    self->loop = loop;
    self->subscription_data = subscription_data;
    self->emission_data = emission_data;
    return self;
}

//------------------------------------------------------------------------------

AuraTask* aura_task_create(AuraTaskProcessor process,
                           AuraLoop* loop,
                           void* subscription_data)
{
    return aura_task_new(process, (AuraTaskFreeFunc) aura_task_free,
                         loop, subscription_data, NULL);
}

//------------------------------------------------------------------------------

AuraTask* aura_task_copy(AuraTask* task)
{
    return aura_task_new(task->process, task->base.free, task->loop,
                         task->subscription_data, task->emission_data);
}

//------------------------------------------------------------------------------

/// Free only task, keep data.
void aura_task_free(AuraTask* self)
{
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(AuraTask));
    free(self);
}

//------------------------------------------------------------------------------

