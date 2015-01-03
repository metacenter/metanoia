// file: event-factory.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_TASK_FACTORY_H__
#define __AURA_EVENT_TASK_FACTORY_H__

#include "event-task.h"
#include "event-dispatcher.h"

AuraTask* factorize_setup_input_devices_task(AuraEventDispatcher* ed);

AuraTask* factorize_setup_device_monitor_task(AuraEventDispatcher* ed);

AuraTask* factorize_backend_gtk_run_task(AuraLoop* loop);

AuraTask* factorize_stop_dispatcher_task(AuraEventDispatcher* dispatcher);

AuraTask* factorize_initialize_wayland_task(AuraLoop* loop);

AuraTask* factorize_initialize_exhibitor_task(AuraLoop* loop);

AuraTask* factorize_initialize_output_collector_task(AuraLoop* loop);

AuraEventData* factorize_signal_event_data(AuraEventDispatcher* dispatcher);

#endif // __AURA_EVENT_TASK_FACTORY_H__

