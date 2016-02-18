// file: event-loop.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EVENT_LOOP_H
#define NOIA_EVENT_LOOP_H

#include "event-task.h"

/// Loop constructor.
NoiaLoop* noia_loop_new(const char* name);

/// Loop destructor.
void noia_loop_free(NoiaLoop* loop);

/// Create and run loop thread.
NoiaResult noia_loop_run(NoiaLoop* loop);

/// Interrupt running loop thread.
void noia_loop_stop(NoiaLoop* loop);

/// Wait for loop thread to exit.
void noia_loop_join(NoiaLoop* loop);

/// Add a task to the queue.
NoiaResult noia_loop_schedule_task(NoiaLoop* loop, NoiaTask* task);

/// Add callback which will be called when loop exits.
NoiaResult noia_loop_add_finalizer(NoiaLoop* self,
                                   NoiaTaskProcessor finalizer,
                                   void* data);

#endif // NOIA_EVENT_LOOP_H

