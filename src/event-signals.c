// file: event-signals.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-signals.h"
#include "event-loop.h"
#include "utils-log.h"
#include "utils-chain.h"

#include <stdlib.h>

//------------------------------------------------------------------------------

typedef struct SignalSubscriber {
    Chain** tab;
} SignalSubscriber;

//------------------------------------------------------------------------------

SignalSubscriber* get_signal_subscriber()
{
    static SignalSubscriber* ss = NULL;
    if (ss != NULL) {
        return ss;
    }

    ss = malloc(sizeof(SignalSubscriber));
    if (ss == NULL) {
        LOG_ERROR("Could not malloc SignalSubscriber!");
        return NULL;
    }

    ss->tab = malloc(SIGNAL_NUM * sizeof(void*));

    return ss;
}

//------------------------------------------------------------------------------

int aura_event_signal_subscribe(AuraSignalNum sig_num, AuraTask* task) {
    if (sig_num >= SIGNAL_NUM) {
        LOG_WARN1("Unknown Signal Number: %d", sig_num);
        return -1;
    }

    if (!task || !task->process) {
        LOG_WARN1("Invalid task!");
        return -1;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return -1;
    }

    Chain* chain = ss->tab[sig_num];
    if (chain == NULL) {
        chain = chain_new(NULL);
        ss->tab[sig_num] = chain;
    }

    LOG_EVNT2("Subscription for signal %d", sig_num);
    chain_append(chain, task);

    return 0;
}

//------------------------------------------------------------------------------

int aura_event_signal_emit(AuraSignalNum sig_num, void* data) {
    if (sig_num >= SIGNAL_NUM) {
        LOG_WARN1("Unknown Signal Number: %d", sig_num);
        return -1;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return -1;
    }

    Chain* chain = ss->tab[sig_num];
    if (chain != NULL) {
        LOG_EVNT4("Signal: emit (num: %d; %d listeners)", sig_num, chain->len);

        Link* link = chain->first;
        while(link) {
            AuraTask* task = link->data;
            if (task) {
                if (task->loop) {
                    LOG_EVNT4("Signal: emited (num: %d)", sig_num);
                    AuraTask* task_copy = aura_task_copy(task);
                    task_copy->data = data;
                    aura_loop_schedule_task(task->loop, task_copy);
                } else {
                    LOG_WARN3("Invalid loop!");
                }
            } else {
                LOG_WARN3("Invalid task!");
            }
            link = link->next;
        }
    } else {
        LOG_EVNT3("Signal: emit (num: %d, no listeners)", sig_num);
    }

    return 0;
}

//------------------------------------------------------------------------------

