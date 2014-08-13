// file: event-task.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-task.h"

#include <malloc.h>

void aura_task_free(AuraTask* self)
{
    if (!self) {
        return;
    }
    // Free only task, keep data
    free(self);
}

