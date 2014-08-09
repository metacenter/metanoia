// file: aura.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils/log.h"
#include "utils/dbus.h"
#include "utils/loop.h"
#include "utils/event_dispatcher.h"
#include "utils/task_factory.h"

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/major.h>
#include <systemd/sd-login.h>

//#include "configuration.h"
#include "keyboard-bindings.h"
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

// FIXME: tmp: just a test
/*void timer_handler (union sigval data)
{
    LOG_DEBUG("TIME'S UP!");
}*/


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
    log_initialize();
    aura_dbus_initalize();

    // TODO: move to configuration.c:aura_config_apply()
    //int i;
    //for (i=0; i < sizeof(scKeyboardBindings)/sizeof(Binding)-1; ++i) {
    //    aura_keyboard_add_binding(&scKeyboardBindings[i]);
    //}

    // Prepare loops and events
    AuraEventDispatcher* dispatcher = aura_event_dispatcher_new();

    AuraLoop* loop_devices = aura_loop_new("devices");
    AuraLoop* loop_window_manager = aura_loop_new("window_manager");
    AuraLoop* loop_keyboard = aura_loop_new("keyboard");

    // Continue initialization asynchronously
    aura_loop_schedule_task(loop_devices,
              task_factory_get_setup_device_monitor_task(dispatcher));
    aura_loop_schedule_task(loop_devices,
              task_factory_get_setup_input_devices_task(dispatcher));
    aura_loop_schedule_task(loop_devices,
              task_factory_get_update_outputs_task(dispatcher));

    // TODO: init surface manager
    aura_loop_schedule_task(loop_window_manager,
              task_factory_get_initialize_wayland_task());

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
    log_finalize();
    return 0;
}
