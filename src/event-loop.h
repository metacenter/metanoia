// file: event-loop.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EVENT_LOOP_H
#define NOIA_EVENT_LOOP_H

#include "event-task.h"

NoiaLoop* noia_loop_new(const char* name);

void noia_loop_free(NoiaLoop* loop);

int noia_loop_run(NoiaLoop* loop);

void noia_loop_stop(NoiaLoop* loop);

void noia_loop_join(NoiaLoop* loop);

int noia_loop_schedule_task(NoiaLoop* loop, NoiaTask* task);

void noia_loop_add_finalizer(NoiaLoop* self, NoiaTaskProcessor finalizer);

#endif // NOIA_EVENT_LOOP_H
