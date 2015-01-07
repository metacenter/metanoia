// file: event-task.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_TASK_H__
#define __AURA_EVENT_TASK_H__

typedef struct AuraLoopPriv AuraLoop;

typedef void (*AuraTaskProcessor) (void*);
typedef void (*AuraTaskFreeFunc) (void*);

typedef struct {
    AuraTaskProcessor process;
    AuraTaskFreeFunc freefunc;
    AuraLoop* loop;
    void* data;
} AuraTask;

AuraTask* aura_task_new(AuraTaskProcessor process,
                        AuraTaskFreeFunc freefunc,
                        AuraLoop* loop,
                        void* data);

AuraTask* aura_task_create(AuraTaskProcessor process,
                           AuraLoop* loop);

AuraTask* aura_task_copy(AuraTask* task);

void aura_task_free(AuraTask *self);

#endif // __AURA_EVENT_TASK_H__

