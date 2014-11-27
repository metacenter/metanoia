// file: event-task-factory.c
// vim: tabstop=4 expandtab colorcolumn=81 list

// TBR

#include "event-task-factory.h"
#include "device-evdev.h"
#include "device-udev.h"
#include "surface.h"
#include "wayland.h"
#include "utils-log.h"

#include <malloc.h>

static AuraTask sDummyTask = {0, NULL, NULL, NULL};

//------------------------------------------------------------------------------

AuraTask* task_factory_get_setup_device_monitor_task(AuraEventDispatcher* ed)
{
    AuraTask* task = malloc(sizeof(AuraTask));
    if (!task) {
        return &sDummyTask;
    }

    task->process = (AuraTaskProcessor) aura_udev_setup_device_monitoring;
    task->freefunc = (AuraTaskFreeFunc) aura_task_free;
    task->loop = NULL;
    task->data = ed;
    return task;
}

//------------------------------------------------------------------------------

AuraTask* task_factory_get_setup_input_devices_task(AuraEventDispatcher* ed)
{
    AuraTask* task = malloc(sizeof(AuraTask));
    if (!task) {
        return &sDummyTask;
    }

    task->process = (AuraTaskProcessor) aura_evdev_setup_input_devices;
    task->freefunc = (AuraTaskFreeFunc) aura_task_free;
    task->loop = NULL;
    task->data = ed;
    return task;
}

//------------------------------------------------------------------------------

AuraTask* task_factory_get_update_outputs_task(AuraEventDispatcher* ed)
{
    AuraTask* task = malloc(sizeof(AuraTask));
    if (!task) {
        return &sDummyTask;
    }

    task->process = (AuraTaskProcessor) aura_update_outputs;
    task->freefunc = (AuraTaskFreeFunc) aura_task_free;
    task->loop = NULL;
    task->data = ed;
    return task;
}

//------------------------------------------------------------------------------

AuraTask* task_factory_get_initialize_wayland_task(AuraLoop* loop)
{
    AuraTask* task = malloc(sizeof(AuraTask));
    if (!task) {
        return &sDummyTask;
    }

    task->process = (AuraTaskProcessor) aura_wayland_initialize;
    task->freefunc = (AuraTaskFreeFunc) aura_task_free;
    task->loop = NULL;
    task->data = loop;
    return task;
}

//------------------------------------------------------------------------------

