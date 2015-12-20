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
    NoiaList** tab;
} SignalSubscriber;

//------------------------------------------------------------------------------

SignalSubscriber* get_signal_subscriber(void)
{
    static SignalSubscriber ss = {NULL};
    if (ss.tab != NULL) {
        return &ss;
    }

    ss.tab = calloc(SIGNAL_NUM, sizeof(NoiaList*));
    return &ss;
}

//------------------------------------------------------------------------------

int noia_event_signal_compare_task_subscription_data(void* data, NoiaTask* task)
{
    if (!task || !task->subscription_data || !data) {
        return 1;
    }
    return task->subscription_data != data;
}

//------------------------------------------------------------------------------

NoiaResult noia_event_signal_subscribe(NoiaSignalNum sig_num, NoiaTask* task)
{
    if (sig_num >= SIGNAL_NUM) {
        LOG_WARN1("Unknown Signal Number: %d", sig_num);
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    if (!task || !task->process) {
        LOG_WARN1("Invalid task!");
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return NOIA_RESULT_ERROR;
    }

    NoiaList* list = ss->tab[sig_num];
    if (list == NULL) {
        list = noia_list_new((NoiaFreeFunc) noia_task_free);
        ss->tab[sig_num] = list;
    }

    LOG_EVNT2("Subscription for signal %d", sig_num);
    noia_list_append(list, task);

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_event_signal_unsubscribe(void* subscription_data)
{
    if (!subscription_data) {
        LOG_WARN1("Invalid data!");
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return NOIA_RESULT_ERROR;
    }

    for (int s = 0; s < SIGNAL_NUM; ++s) {
        NoiaList* list = ss->tab[s];
        if (list) {
            noia_list_remove_all(list, subscription_data, (NoiaCompareFunc)
                              noia_event_signal_compare_task_subscription_data);
        }
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_event_signal_emit(NoiaSignalNum sig_num, NoiaObject* object)
{
    if (sig_num >= SIGNAL_NUM) {
        LOG_WARN1("Unknown Signal Number: %d", sig_num);
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return NOIA_RESULT_ERROR;
    }

    NoiaList* list = ss->tab[sig_num];
    if (list) {
        LOG_EVNT4("Signal: emit (num: %d; %d listeners)",
                  sig_num, noia_list_len(list));

        FOR_EACH(list, link) {
            NoiaTask* task = link->data;
            if (task) {
                if (task->loop) {
                    LOG_EVNT4("Signal: emited (num: %d)", sig_num);
                    /// @todo Do not allocate memory here.
                    NoiaTask* task_copy = noia_task_copy(task);
                    task_copy->emission_data = object;
                    noia_object_ref(object);
                    noia_loop_schedule_task(task->loop, task_copy);
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

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_event_signal_emit_int(NoiaSignalNum sig_num, intptr_t value)
{
    NoiaObject* object = (NoiaObject*) noia_int_create(value);
    NoiaResult result = noia_event_signal_emit(sig_num, object);
    noia_object_unref(object);
    return result;
}

//------------------------------------------------------------------------------

void noia_event_signal_clear_all_substriptions(void)
{
    SignalSubscriber* ss = get_signal_subscriber();
    if (ss == NULL) {
        LOG_ERROR("Invalid Signal Subscriber!");
        return;
    }

    for (int s = 0; s < SIGNAL_NUM; ++s) {
        if (ss->tab[s]) {
            noia_list_free(ss->tab[s]);
        }
    }
    free(ss->tab);
    ss->tab = NULL;
}

//------------------------------------------------------------------------------

