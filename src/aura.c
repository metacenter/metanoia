// file: aura.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-log.h"
#include "utils-dbus.h"
#include "utils-environment.h"
#include "event-dispatcher.h"
#include "event-loop.h"
#include "event-task-factory.h"
#include "config.h"

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/major.h>
#include <systemd/sd-login.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/vt.h>
#include <linux/kd.h>

void sig_handler(int sig, siginfo_t *si, void *arg)
{
    switch (sig) {
        case SIGINT:
        case SIGSEGV:
            LOG_INFO1("Signal '%d' received", sig);
            aura_print_backtrace();
            abort();
        default:
            LOG_INFO2("Unhandled signal: '%d'", sig);
    }
}

int main()
{
    // Initialize signal hadler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sig_handler;
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);

    // Initialization
    aura_environment_setup();
    aura_config_apply();
    if (!aura_config().run_in_test_window) {
        aura_dbus_initalize();
    }

    // Prepare loops and events
    AuraEventDispatcher* dispatcher = aura_event_dispatcher_new();

    AuraLoop* loop_devices = aura_loop_new("devices");
    AuraLoop* loop_window_manager = aura_loop_new("window_manager");
    AuraLoop* loop_keyboard = aura_loop_new("keyboard");

    // Continue initialization asynchronously
    if (!aura_config().run_in_test_window) {
        aura_loop_schedule_task(loop_devices,
                  task_factory_get_setup_device_monitor_task(dispatcher));
        aura_loop_schedule_task(loop_devices,
                  task_factory_get_setup_input_devices_task(dispatcher));
        aura_loop_schedule_task(loop_devices,
                  task_factory_get_update_outputs_task(dispatcher));
    } else {
        aura_loop_schedule_task(loop_devices,
                  task_factory_get_backend_gtk_initialize_task(loop_devices));
    }

    // TODO: init surface manager
    aura_loop_schedule_task(loop_window_manager,
              task_factory_get_initialize_wayland_task(loop_window_manager));

    // Start threads
    aura_loop_run(loop_devices);
    aura_loop_run(loop_window_manager);
    aura_loop_run(loop_keyboard);

    // Receive and dispatch events
    aura_event_dispatcher_start(dispatcher);

    // Stop threads
    aura_loop_stop(loop_keyboard);
    aura_loop_stop(loop_window_manager);
    aura_loop_stop(loop_devices);

    // Finalize
    aura_loop_free(loop_keyboard);
    aura_loop_free(loop_window_manager);
    aura_loop_free(loop_devices);

    aura_dbus_finalize();
    aura_environment_cleanup();
    return 0;
}
