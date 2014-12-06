// file: aura.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-log.h"
#include "utils-dbus.h"
#include "utils-environment.h"
#include "event-dispatcher.h"
#include "event-loop.h"
#include "event-factory.h"
#include "config.h"

int main()
{
    AuraTask* task;
    AuraEventData* event_data;

    // Initialization
    aura_environment_setup();
    aura_config_apply();
    if (!aura_config().run_in_test_window) {
        aura_dbus_initalize();
    }

    // Prepare event dispatcher
    AuraEventDispatcher* dispatcher = aura_event_dispatcher_new();
    aura_event_dispatcher_initialize(dispatcher);

    // Prepare loops and events
    AuraLoop* loop_devices = aura_loop_new("devices");
    AuraLoop* loop_window_manager = aura_loop_new("window_manager");
    AuraLoop* loop_keyboard = aura_loop_new("keyboard");

    // Continue initialization asynchronously
    task = factorize_setup_device_monitor_task(dispatcher);
    aura_loop_schedule_task(loop_devices, task);

    task = factorize_setup_input_devices_task(dispatcher);
    aura_loop_schedule_task(loop_devices, task);

    if (aura_config().run_in_test_window) {
        task = factorize_backend_gtk_run_task(loop_devices);
        aura_loop_schedule_task(loop_devices, task);
    }

    task = factorize_initialize_output_collector_task(loop_devices);
    aura_loop_schedule_task(loop_devices, task);

    task = factorize_initialize_surface_manager_task(loop_window_manager);
    aura_loop_schedule_task(loop_window_manager, task);

    task = factorize_initialize_wayland_task(loop_window_manager);
    aura_loop_schedule_task(loop_window_manager, task);

    // Start threads
    aura_loop_run(loop_devices);
    aura_loop_run(loop_window_manager);
    aura_loop_run(loop_keyboard);

    // Receive and dispatch events (main loop)
    event_data = factorize_signal_event_data(dispatcher);
    aura_event_dispatcher_add_event_source(dispatcher, event_data);
    aura_environment_block_system_signals();
    aura_event_dispatcher_start(dispatcher);
    aura_environment_unblock_system_signals();

    // Stop and join threads
    aura_loop_stop(loop_keyboard);
    aura_loop_stop(loop_window_manager);
    aura_loop_stop(loop_devices);
    aura_loop_join(loop_keyboard);
    aura_loop_join(loop_window_manager);
    aura_loop_join(loop_devices);

    // Finalize
    aura_loop_free(loop_keyboard);
    aura_loop_free(loop_window_manager);
    aura_loop_free(loop_devices);

    aura_dbus_finalize();
    aura_environment_cleanup();
    return 0;
}
