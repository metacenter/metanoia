// file: event-loop.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-loop.h"
#include "utils-log.h"
#include "utils-list.h"
#include "utils-environment.h"

#include <errno.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

struct NoiaLoopPriv {
    char* name;
    pthread_t thread;
    pthread_mutex_t schedule_mutex;
    pthread_mutex_t process_mutex;
    pthread_cond_t condition;
    bool run;
    NoiaList* task_list;
    NoiaList* finalizers;
};

//------------------------------------------------------------------------------

NoiaLoop* noia_loop_new(const char* name)
{
    NoiaLoop* self = malloc(sizeof(NoiaLoop));
    if (!self) {
        return NULL;
    }

    self->name = strdup(name);
    pthread_mutex_init(&self->schedule_mutex, NULL);
    pthread_mutex_init(&self->process_mutex, NULL);
    pthread_cond_init(&self->condition, NULL);
    self->run = 0;
    self->task_list = noia_list_new(NULL);
    self->finalizers = noia_list_new(NULL);
    return self;
}

//------------------------------------------------------------------------------

void noia_loop_free(NoiaLoop* self)
{
    if (!self) {
        return;
    }

    if (self->name) {
        free(self->name);
    }
    noia_list_free(self->finalizers);
    noia_list_free(self->task_list);
    memset(self, 0, sizeof(NoiaLoop));
    free(self);
}

//------------------------------------------------------------------------------

void noia_loop_process_task(NoiaTask* task)
{
    if (task) {
        if (task->process) {
            LOG_EVNT4("Loop: processing task");
            task->process(task->emission_data);
        } else {
            LOG_ERROR("Invalid task processor!");
        }
        noia_object_unref((NoiaObject*) task);
    } else {
        LOG_ERROR("Invalid task!");
    }
}

//------------------------------------------------------------------------------

void* noia_loop_thread_loop(void* data)
{
    NoiaLoop* self = (NoiaLoop*) data;
    if (!self) {
        return NULL;
    }

    LOG_INFO1("Threads: starting loop '%s'", self->name);
    noia_environment_on_enter_new_thread(self->thread, self->name);

    self->run = 1;
    pthread_mutex_lock(&self->process_mutex);
    while (self->run) {
        while (noia_list_len(self->task_list) > 0) {
            pthread_mutex_lock(&self->schedule_mutex);
            NoiaTask* task = noia_list_pop(self->task_list);
            pthread_mutex_unlock(&self->schedule_mutex);
            noia_loop_process_task(task);
        }
        pthread_cond_wait(&self->condition, &self->process_mutex);
    }

    LOG_INFO1("Threads: finalizing loop '%s'", self->name);
    FOR_EACH_REVERSE (self->finalizers, link) {
        noia_loop_process_task((NoiaTask*) link->data);
    }

    LOG_INFO1("Threads: stopped loop '%s'", self->name);
    return NULL;
}

//------------------------------------------------------------------------------

int noia_loop_run(NoiaLoop* self)
{
    if (!self) {
        return -ENOMEM;
    }

    return pthread_create(&self->thread, NULL, noia_loop_thread_loop, self);
}

//------------------------------------------------------------------------------

void noia_loop_stop(NoiaLoop* self)
{
    if (!self) {
        return;
    }

    LOG_INFO1("Threads: stopping loop '%s'", self->name);
    pthread_mutex_lock(&self->process_mutex);
    self->run = 0;
    pthread_mutex_unlock(&self->process_mutex);
    pthread_cond_signal(&self->condition);
}

//------------------------------------------------------------------------------

void noia_loop_join(NoiaLoop* self)
{
    if (!self) {
        return;
    }
    pthread_join(self->thread, NULL);
}

//------------------------------------------------------------------------------

int noia_loop_schedule_task(NoiaLoop* self, NoiaTask* task)
{
    if (!self || !task) {
        LOG_ERROR("Invalid Loop or Task!");
        return -ENOMEM;
    }

    pthread_mutex_lock(&self->schedule_mutex);
    noia_list_append(self->task_list, task);
    pthread_mutex_unlock(&self->schedule_mutex);
    pthread_cond_signal(&self->condition);
    return 1;
}

//------------------------------------------------------------------------------

void noia_loop_add_finalizer(NoiaLoop* self, NoiaTaskProcessor finalizer)
{
    noia_list_append(self->finalizers, noia_task_new(finalizer,
                    (NoiaFreeFunc) noia_task_free, NULL, NULL, NULL));
}

//------------------------------------------------------------------------------

