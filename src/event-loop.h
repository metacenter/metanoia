// file: event-loop.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_LOOP_H__
#define __AURA_EVENT_LOOP_H__

#include "event-task.h"

typedef struct AuraLoopPriv AuraLoop;

AuraLoop* aura_loop_new(const char* name);
void aura_loop_free(AuraLoop* loop);

int aura_loop_run(AuraLoop* loop);
void aura_loop_stop(AuraLoop* loop);
void aura_loop_join(AuraLoop* loop);
int aura_loop_schedule_task(AuraLoop* loop, AuraTask* task);

#endif // __AURA_EVENT_LOOP_H__
