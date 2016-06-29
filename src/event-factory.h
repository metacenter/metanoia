// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EVENT_TASK_FACTORY_H
#define NOIA_EVENT_TASK_FACTORY_H

#include "event-task.h"
#include "event-dispatcher.h"
#include "surface-coordinator.h"

typedef struct {
    NoiaCoordinator* coordinator;
    NoiaEventDispatcher* ed;
} NoiaFactoryBundle;

NoiaTask* factorize_setup_input_devices_task(NoiaEventDispatcher* ed);

NoiaTask* factorize_setup_device_monitor_task(NoiaEventDispatcher* ed);

NoiaTask* factorize_backend_offscreen_run_task(NoiaLoop* loop);

NoiaTask* factorize_initialize_wayland_task(NoiaLoop* loop,
                                            NoiaCoordinator* coordinator);

NoiaTask* factorize_initialize_exhibitor_task(NoiaLoop* loop,
                                              NoiaFactoryBundle* bundle);

NoiaTask* factorize_initialize_output_collector_task(NoiaLoop* loop);

NoiaEventData* factorize_signal_event_data(NoiaEventDispatcher* dispatcher);

#endif // NOIA_EVENT_TASK_FACTORY_H

