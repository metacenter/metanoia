// file: task.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_TASK_H__
#define __AURA_TASK_H__

typedef struct AuraTask AuraTask;

typedef void (*AuraTaskProcessor) (void*);
typedef void (*AuraTaskFreeFunc) (void*);

struct AuraTask {
    AuraTaskProcessor process;
    AuraTaskFreeFunc freefunc;
    void* data;
};

void aura_task_free(AuraTask *self);

#endif // __AURA_TASK_H__

