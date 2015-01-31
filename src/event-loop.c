// file: event-loop.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-loop.h"
#include "utils-log.h"
#include "utils-chain.h"
#include "utils-environment.h"

#include <errno.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

struct AuraLoopPriv {
    char* name;
    pthread_t thread;
    pthread_mutex_t schedule_mutex;
    pthread_mutex_t process_mutex;
    pthread_cond_t condition;
    bool run;
    Chain* task_chain;
    AuraTaskProcessor finalize;
};

//------------------------------------------------------------------------------

AuraLoop* aura_loop_new(const char* name)
{
    AuraLoop* self = malloc(sizeof(AuraLoop));
    if (!self) {
        return NULL;
    }

    self->name = strdup(name);
    pthread_mutex_init(&self->schedule_mutex, NULL);
    pthread_mutex_init(&self->process_mutex, NULL);
    pthread_cond_init(&self->condition, NULL);
    self->run = 0;
    self->task_chain = chain_new(NULL);
    self->finalize = NULL;
    return self;
}

//------------------------------------------------------------------------------

void aura_loop_free(AuraLoop* self)
{
    if (!self) {
        return;
    }

    if (self->name) {
        free(self->name);
    }
    chain_free(self->task_chain);
    free(self);
}

//------------------------------------------------------------------------------

static void* aura_loop_thread_loop(void* data)
{
    AuraLoop* self = (AuraLoop*) data;
    if (!self) {
        return NULL;
    }

    LOG_INFO1("Threads: starting loop '%s'", self->name);
    aura_environment_on_enter_new_thread(self->thread, self->name);

    self->run = 1;
    pthread_mutex_lock(&self->process_mutex);
    while (self->run) {
        while (chain_len(self->task_chain) > 0) {
            pthread_mutex_lock(&self->schedule_mutex);
            AuraTask* task = chain_pop(self->task_chain);
            pthread_mutex_unlock(&self->schedule_mutex);
            if (task) {
                if (task->process) {
                    LOG_EVNT4("Loop: processing task");
                    task->process(task->data);
                } else {
                    LOG_ERROR("Invalid task processor!");
                }
                if (task->freefunc) {
                    task->freefunc(task);
                }
            } else {
                LOG_ERROR("Invalid task!");
            }
        }

        pthread_cond_wait(&self->condition, &self->process_mutex);
    }

    if (self->finalize) {
        self->finalize(self);
    }

    LOG_INFO1("Threads: stopped loop '%s'", self->name);
    return NULL;
}

//------------------------------------------------------------------------------

int aura_loop_run(AuraLoop* self)
{
    if (!self) {
        return -ENOMEM;
    }

    return pthread_create(&self->thread, NULL, aura_loop_thread_loop, self);
}

//------------------------------------------------------------------------------

void aura_loop_stop(AuraLoop* self)
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

void aura_loop_join(AuraLoop* self)
{
    if (!self) {
        return;
    }
    pthread_join(self->thread, NULL);
}

//------------------------------------------------------------------------------

int aura_loop_schedule_task(AuraLoop* self, AuraTask* task)
{
    if (!self || !task) {
        LOG_ERROR("Invalid Loop or Task!");
        return -ENOMEM;
    }

    pthread_mutex_lock(&self->schedule_mutex);
    chain_append(self->task_chain, task);
    pthread_mutex_unlock(&self->schedule_mutex);
    pthread_cond_signal(&self->condition);
    return 1;
}

//------------------------------------------------------------------------------

void aura_loop_set_finalizer(AuraLoop* self, AuraTaskProcessor finalizer)
{
    self->finalize = finalizer;
}

//------------------------------------------------------------------------------

