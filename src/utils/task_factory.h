// file: task_factory.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_TASK_FACTORY_H__
#define __AURA_TASK_FACTORY_H__

#include "utils/task.h"
#include "utils/event_dispatcher.h"

AuraTask* task_factory_get_setup_input_devices_task(AuraEventDispatcher* ed);

AuraTask* task_factory_get_setup_device_monitor_task(AuraEventDispatcher* ed);

AuraTask* task_factory_get_update_outputs_task();

AuraTask* task_factory_get_initialize_wayland_task();

#endif // __AURA_TASK_FACTORY_H__

