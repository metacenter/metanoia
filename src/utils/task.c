// file: task.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "task.h"

#include <malloc.h>

void aura_task_free(AuraTask* self)
{
    if (!self) {
        return;
    }
    // Free only task, keep data
    free(self);
}

