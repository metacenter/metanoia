// file: event-factory.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EVENT_TASK_FACTORY_H
#define NOIA_EVENT_TASK_FACTORY_H

#include "event-task.h"
#include "event-dispatcher.h"

NoiaTask* factorize_setup_input_devices_task(NoiaEventDispatcher* ed);

NoiaTask* factorize_setup_device_monitor_task(NoiaEventDispatcher* ed);

NoiaTask* factorize_backend_offscreen_run_task(NoiaLoop* loop);

NoiaTask* factorize_stop_dispatcher_task(NoiaEventDispatcher* dispatcher);

NoiaTask* factorize_initialize_wayland_task(NoiaLoop* loop);

NoiaTask* factorize_initialize_exhibitor_task(NoiaLoop* loop);

NoiaTask* factorize_initialize_output_collector_task(NoiaLoop* loop);

NoiaEventData* factorize_signal_event_data(NoiaEventDispatcher* dispatcher);

#endif // NOIA_EVENT_TASK_FACTORY_H

