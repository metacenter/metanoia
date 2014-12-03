// file: event-factory.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "event-factory.h"
#include "event-dispatcher.h"
#include "device-evdev.h"
#include "device-udev.h"
#include "backend-gtk.h"
#include "surface.h"
#include "wayland.h"
#include "utils-environment.h"

#include <malloc.h>
#include <signal.h>
#include <sys/signalfd.h>

//------------------------------------------------------------------------------

AuraTask* factorize_setup_device_monitor_task(AuraEventDispatcher* ed)
{
    return aura_task_new((AuraTaskProcessor) aura_udev_setup_device_monitoring,
                         (AuraTaskFreeFunc) aura_task_free, NULL, ed);
}

//------------------------------------------------------------------------------

AuraTask* factorize_setup_input_devices_task(AuraEventDispatcher* ed)
{
    return aura_task_new((AuraTaskProcessor) aura_evdev_setup_input_devices,
                         (AuraTaskFreeFunc) aura_task_free, NULL, ed);
}

//------------------------------------------------------------------------------

AuraTask* factorize_update_outputs_task(AuraEventDispatcher* ed)
{
    return aura_task_new((AuraTaskProcessor) aura_update_outputs,
                         (AuraTaskFreeFunc) aura_task_free, NULL, ed);
}

//------------------------------------------------------------------------------

AuraTask* factorize_initialize_wayland_task(AuraLoop* loop)
{
    return aura_task_new((AuraTaskProcessor) aura_wayland_initialize,
                         (AuraTaskFreeFunc) aura_task_free, NULL, loop);
}

//------------------------------------------------------------------------------

AuraTask* factorize_backend_gtk_run_task(AuraLoop* loop)
{
    return aura_task_new((AuraTaskProcessor) aura_backend_gtk_run,
                         (AuraTaskFreeFunc) aura_task_free, NULL, loop);
}

//------------------------------------------------------------------------------

AuraEventData* factorize_signal_event_data(AuraEventDispatcher* dispatcher)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    int fd = signalfd(-1, &mask, 0);
    if (fd == -1) {
        return NULL;
    }

    AuraEventData* data = malloc(sizeof(AuraEventData));
    data->handler = aura_event_dispatcher_default_signal_handler;
    data->data.ptr = dispatcher;
    data->fd = fd;

    return data;
}

//------------------------------------------------------------------------------

