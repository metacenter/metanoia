// file: event-task.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_TASK_H__
#define __AURA_EVENT_TASK_H__

#include "utils-object.h"

typedef struct AuraLoopPriv AuraLoop;

typedef void (*AuraTaskProcessor) (void*);
typedef void (*AuraTaskFreeFunc) (void*);

typedef struct {
    AuraObject base;
    AuraTaskProcessor process;
    AuraLoop* loop;
    void* subscription_data;
    void* emission_data;
} AuraTask;

AuraTask* aura_task_new(AuraTaskProcessor process,
                        AuraTaskFreeFunc free,
                        AuraLoop* loop,
                        void* subscription_data,
                        void* emission_data);

AuraTask* aura_task_create(AuraTaskProcessor process,
                           AuraLoop* loop,
                           void* subscription_data);

AuraTask* aura_task_copy(AuraTask* task);

void aura_task_free(AuraTask *self);

#endif // __AURA_EVENT_TASK_H__

