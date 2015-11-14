// file: metanoia.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-log.h"
#include "utils-dbus.h"
#include "utils-environment.h"
#include "event-dispatcher.h"
#include "event-loop.h"
#include "event-signals.h"
#include "event-factory.h"
#include "surface-manager.h"
#include "config.h"

int main(int argc, char** argv)
{
    NoiaTask* task;
    NoiaEventData* event_data;

    // Initialization
    noia_environment_set_thread_name(0, "noia:main");
    noia_environment_setup("log");
    noia_config_apply(argc, argv);
    noia_dbus_initalize();

    // Prepare event dispatcher
    NoiaEventDispatcher* dispatcher = noia_event_dispatcher_new();
    noia_event_dispatcher_initialize(dispatcher);

    // Prepare loops and events
    NoiaLoop* loop_devices = noia_loop_new("noia:devices");
    NoiaLoop* loop_displays = noia_loop_new("noia:displays");
    NoiaLoop* loop_keyboard = noia_loop_new("noia:keyboard");

    // Continue initialization asynchronously
    task = factorize_setup_device_monitor_task(dispatcher);
    noia_loop_schedule_task(loop_devices, task);

    task = factorize_setup_input_devices_task(dispatcher);
    noia_loop_schedule_task(loop_devices, task);

    if (noia_settings()->run_in_test_window) {
        task = factorize_backend_gtk_run_task(loop_devices);
        noia_loop_schedule_task(loop_devices, task);
    }

    task = factorize_initialize_output_collector_task(loop_devices);
    noia_loop_schedule_task(loop_devices, task);

    task = factorize_initialize_wayland_task(loop_displays);
    noia_loop_schedule_task(loop_displays, task);

    task = factorize_initialize_exhibitor_task(loop_displays);
    noia_loop_schedule_task(loop_displays, task);

    // Start threads
    noia_loop_run(loop_devices);
    noia_loop_run(loop_displays);
    noia_loop_run(loop_keyboard);

    // Receive and dispatch events (main loop)
    event_data = factorize_signal_event_data(dispatcher);
    noia_event_dispatcher_add_event_source(dispatcher, event_data);
    noia_environment_block_system_signals();
    noia_event_dispatcher_start(dispatcher);
    noia_environment_unblock_system_signals();

    // Stop and join threads
    noia_loop_stop(loop_keyboard);
    noia_loop_stop(loop_displays);
    noia_loop_stop(loop_devices);
    noia_loop_join(loop_keyboard);
    noia_loop_join(loop_displays);
    noia_loop_join(loop_devices);

    // Finalize
    noia_loop_free(loop_keyboard);
    noia_loop_free(loop_displays);
    noia_loop_free(loop_devices);
    noia_event_dispatcher_free(dispatcher);
    noia_event_signal_clear_all_substriptions();
    noia_surface_clear_all();

    noia_dbus_finalize();
    noia_config_finalize();
    noia_environment_cleanup();
    return 0;
}
