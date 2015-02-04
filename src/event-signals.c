// file: event-signals.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-signals.h"
#include "event-loop.h"
#include "utils-log.h"
#include "utils-chain.h"
#include "global-objects.h"

#include <stdlib.h>

//------------------------------------------------------------------------------

typedef struct SignalSubscriber {
    Chain** tab;
} SignalSubscriber;

//------------------------------------------------------------------------------

SignalSubscriber* get_signal_subscriber()
{
    static SignalSubscriber ss = {NULL};
    if (ss.tab != NULL) {
        return &ss;
    }

    ss.tab = calloc(SIGNAL_NUM, sizeof(Chain*));
    return &ss;
}

//------------------------------------------------------------------------------

AuraResult aura_event_signal_subscribe(AuraSignalNum sig_num, AuraTask* task) {
    if (sig_num >= SIGNAL_NUM) {
        LOG_WARN1("Unknown Signal Number: %d", sig_num);
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    if (!task || !task->process) {
        LOG_WARN1("Invalid task!");
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return AURA_RESULT_ERROR;
    }

    Chain* chain = ss->tab[sig_num];
    if (chain == NULL) {
        chain = chain_new((AuraFreeFunc) aura_task_free);
        ss->tab[sig_num] = chain;
    }

    LOG_EVNT2("Subscription for signal %d", sig_num);
    chain_append(chain, task);

    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

AuraResult aura_event_signal_emit(AuraSignalNum sig_num, AuraObject* object) {
    if (sig_num >= SIGNAL_NUM) {
        LOG_WARN1("Unknown Signal Number: %d", sig_num);
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return AURA_RESULT_ERROR;
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
                    task_copy->data = object;
                    aura_object_ref(object);
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

    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

AuraResult aura_event_signal_emit_int(AuraSignalNum sig_num, intptr_t value)
{
    AuraObject* object = (AuraObject*) aura_int_create(value);
    AuraResult result = aura_event_signal_emit(sig_num, object);
    aura_object_unref(object);
    return result;
}

//------------------------------------------------------------------------------

void aura_event_signal_clear_all_substriptions()
{
    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return;
    }

    for (int s = 0; s < SIGNAL_NUM; ++s) {
        if (ss->tab[s]) {
            chain_free(ss->tab[s]);
        }
    }
    free(ss->tab);
    ss->tab = NULL;
}

//------------------------------------------------------------------------------

