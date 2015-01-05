// file: event-loop.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-loop.h"
#include "utils-log.h"
#include "utils-chain.h"
#include "utils-environment.h"

#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char* name;
    pthread_t thread;
    pthread_mutex_t schedule_mutex;
    pthread_mutex_t process_mutex;
    pthread_cond_t condition;
    bool run;
    Chain* task_chain;
} AuraLoopPriv;

//-------------------------------------------------------------------------

AuraLoop* aura_loop_new(const char* name)
{
    AuraLoopPriv* mine = malloc(sizeof(AuraLoopPriv));
    if (!mine) {
        return NULL;
    }

    mine->name = strdup(name);
    pthread_mutex_init(&mine->schedule_mutex, NULL);
    pthread_mutex_init(&mine->process_mutex, NULL);
    pthread_cond_init(&mine->condition, NULL);
    mine->run = 0;
    mine->task_chain = chain_new(NULL);

    return (AuraLoop*) mine;
}

//-------------------------------------------------------------------------

void aura_loop_free(AuraLoop* self)
{
    AuraLoopPriv* mine = (AuraLoopPriv*) self;
    if (!mine) {
        return;
    }

    if (mine->name) {
        free(mine->name);
    }
    chain_free(mine->task_chain);
    free(mine);
}

//-------------------------------------------------------------------------

static void* loop(void* data)
{
    AuraLoopPriv* mine = (AuraLoopPriv*) data;
    if (!mine) {
        return NULL;
    }

    LOG_INFO1("Threads: starting loop '%s'", mine->name);
    aura_environment_block_system_signals();

    mine->run = 1;
    pthread_mutex_lock(&mine->process_mutex);
    while (mine->run) {
        while (chain_len(mine->task_chain) > 0) {
            pthread_mutex_lock(&mine->schedule_mutex);
            AuraTask* task = chain_pop(mine->task_chain);
            pthread_mutex_unlock(&mine->schedule_mutex);
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

        pthread_cond_wait(&mine->condition, &mine->process_mutex);
    }

    LOG_INFO1("Threads: stopped loop '%s'", mine->name);
    return NULL;
}

//-------------------------------------------------------------------------

int aura_loop_run(AuraLoop* self)
{
    AuraLoopPriv* mine = (AuraLoopPriv*) self;
    if (!mine) {
        return -ENOMEM;
    }

    return pthread_create(&mine->thread, NULL, loop, mine);
}

//-------------------------------------------------------------------------

void aura_loop_stop(AuraLoop* self)
{
    AuraLoopPriv* mine = (AuraLoopPriv*) self;
    if (!mine) {
        return;
    }

    LOG_INFO1("Threads: stopping loop '%s'", mine->name);
    pthread_mutex_lock(&mine->process_mutex);
    mine->run = 0;
    pthread_mutex_unlock(&mine->process_mutex);
    pthread_cond_signal(&mine->condition);
}

//-------------------------------------------------------------------------

void aura_loop_join(AuraLoop* self)
{
    AuraLoopPriv* mine = (AuraLoopPriv*) self;
    if (!mine) {
        return;
    }
    pthread_join(mine->thread, NULL);
}

//-------------------------------------------------------------------------

int aura_loop_schedule_task(AuraLoop* self, AuraTask* task)
{
    AuraLoopPriv* mine = (AuraLoopPriv*) self;
    if (!mine || !task) {
        LOG_ERROR("Invalid Loop or Task!");
        return -ENOMEM;
    }

    pthread_mutex_lock(&mine->schedule_mutex);
    chain_append(mine->task_chain, task);
    pthread_mutex_unlock(&mine->schedule_mutex);
    pthread_cond_signal(&mine->condition);
    return 1;
}

//-------------------------------------------------------------------------

