// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "event-factory.h"
#include "event-dispatcher.h"
#include "device-udev.h"
#include "backend-offscreen.h"
#include "output-collector.h"
#include "exhibitor-module.h"
#include "wayland-module.h"
#include "utils-environment.h"

#include <malloc.h>
#include <signal.h>
#include <sys/signalfd.h>

//------------------------------------------------------------------------------

NoiaTask* factorize_setup_input_devices_task(NoiaEventDispatcher* ed)
{
    return noia_task_new((NoiaTaskProcessor) noia_udev_setup_input_devices,
                         (NoiaTaskFreeFunc) noia_task_free, NULL, NULL, ed);
}

//------------------------------------------------------------------------------

NoiaTask* factorize_setup_device_monitor_task(NoiaEventDispatcher* ed)
{
    return noia_task_new((NoiaTaskProcessor) noia_udev_setup_device_monitoring,
                         (NoiaTaskFreeFunc) noia_task_free, NULL, NULL, ed);
}

//------------------------------------------------------------------------------

NoiaTask* factorize_backend_offscreen_run_task(NoiaLoop* loop)
{
    return noia_task_new((NoiaTaskProcessor) noia_backend_offscreen_run,
                         (NoiaTaskFreeFunc) noia_task_free, NULL, NULL, loop);
}

//------------------------------------------------------------------------------

NoiaTask* factorize_initialize_wayland_task(NoiaLoop* loop,
                                            NoiaCoordinator* coordinator)
{
    return noia_task_new((NoiaTaskProcessor) noia_wayland_initialize,
                         (NoiaTaskFreeFunc) noia_task_free, NULL,
                         coordinator, loop);
}

//------------------------------------------------------------------------------

NoiaTask* factorize_initialize_exhibitor_task(NoiaLoop* loop,
                                              NoiaCoordinator* coordinator)
{
    return noia_task_new((NoiaTaskProcessor) noia_exhibitor_initialize,
                         (NoiaTaskFreeFunc) noia_task_free, NULL,
                         coordinator, loop);
}

//------------------------------------------------------------------------------

NoiaTask* factorize_initialize_output_collector_task(NoiaLoop* loop)
{
    return noia_task_new((NoiaTaskProcessor) noia_output_collector_initialize,
                         (NoiaTaskFreeFunc) noia_task_free, NULL, NULL, loop);
}

//------------------------------------------------------------------------------

NoiaEventData* factorize_signal_event_data(NoiaEventDispatcher* dispatcher)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    int fd = signalfd(-1, &mask, 0);
    if (fd == -1) {
        return NULL;
    }

    return noia_event_data_create(fd,
                                  noia_event_dispatcher_default_signal_handler,
                                  NULL, 0x0, dispatcher);
}

//------------------------------------------------------------------------------

