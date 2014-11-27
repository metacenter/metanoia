// file: event-task.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-task.h"
#include "utils-log.h"

#include <malloc.h>

static AuraTask sDummyTask = {0, NULL, NULL, NULL};

void aura_task_free(AuraTask* self)
{
    if (!self) {
        return;
    }
    // Free only task, keep data
    free(self);
}

//------------------------------------------------------------------------------

AuraTask* aura_task_new(AuraTaskProcessor process,
                        AuraTaskFreeFunc freefunc,
                        AuraLoop* loop,
                        void* data)
{
    AuraTask* task = malloc(sizeof(AuraTask));
    if (!task) {
        return &sDummyTask;
    }

    task->process = process;
    task->freefunc = freefunc;
    task->loop = loop;
    task->data = data;
    return task;
}

//------------------------------------------------------------------------------

AuraTask* aura_task_create(AuraTaskProcessor process,
                           AuraLoop* loop)
{
    return aura_task_new(process, (AuraTaskFreeFunc)aura_task_free, loop, NULL);
}

//------------------------------------------------------------------------------

