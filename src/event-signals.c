// file: event-signals.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-signals.h"
#include "event-loop.h"
#include "utils-log.h"
#include "utils-list.h"
#include "global-objects.h"

#include <stdlib.h>

//------------------------------------------------------------------------------

typedef struct SignalSubscriber {
    AuraList** tab;
} SignalSubscriber;

//------------------------------------------------------------------------------

SignalSubscriber* get_signal_subscriber()
{
    static SignalSubscriber ss = {NULL};
    if (ss.tab != NULL) {
        return &ss;
    }

    ss.tab = calloc(SIGNAL_NUM, sizeof(AuraList*));
    return &ss;
}

//------------------------------------------------------------------------------

int aura_event_signal_compare_task_subscription_data(void* data, AuraTask* task)
{
    if (!task || !task->subscription_data || !data) {
        return 1;
    }
    return task->subscription_data != data;
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

    AuraList* list = ss->tab[sig_num];
    if (list == NULL) {
        list = aura_list_new((AuraFreeFunc) aura_task_free);
        ss->tab[sig_num] = list;
    }

    LOG_EVNT2("Subscription for signal %d", sig_num);
    aura_list_append(list, task);

    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

AuraResult aura_event_signal_unsubscribe(void* subscription_data)
{
    if (!subscription_data) {
        LOG_WARN1("Invalid data!");
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return AURA_RESULT_ERROR;
    }

    for (int s = 0; s < SIGNAL_NUM; ++s) {
        AuraList* list = ss->tab[s];
        if (list) {
            aura_list_remove_all(list, subscription_data, (AuraCompareFunc)
                              aura_event_signal_compare_task_subscription_data);
        }
    }

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

    AuraList* list = ss->tab[sig_num];
    if (list) {
        LOG_EVNT4("Signal: emit (num: %d; %d listeners)",
                  sig_num, aura_list_len(list));

        FOR_EACH(list, link) {
            AuraTask* task = link->data;
            if (task) {
                if (task->loop) {
                    LOG_EVNT4("Signal: emited (num: %d)", sig_num);
                    AuraTask* task_copy = aura_task_copy(task);
                    task_copy->emission_data = object;
                    aura_object_ref(object);
                    aura_loop_schedule_task(task->loop, task_copy);
                } else {
                    LOG_WARN3("Invalid loop!");
                }
            } else {
                LOG_WARN3("Invalid task!");
            }
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
            aura_list_free(ss->tab[s]);
        }
    }
    free(ss->tab);
    ss->tab = NULL;
}

//------------------------------------------------------------------------------

