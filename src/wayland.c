// file: wayland.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland.h"
#include "wayland-protocol-compositor.h"
#include "wayland-protocol-shell.h"
#include "wayland-protocol-seat.h"
#include "wayland-protocol-xdg-shell.h"
#include "wayland-protocol-output.h"
#include "wayland-state.h"

#include "utils-log.h"
#include "utils-environment.h"
#include "event-signals.h"

#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <wayland-server.h>

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

static const int scVersion = 3;
static const char* scSocketName = "wayland-0";

static struct wl_display* wayland_display;

//------------------------------------------------------------------------------

struct wl_display* get_wayland_display()
{
    return wayland_display;
}

//------------------------------------------------------------------------------

static void* display_run(void* data)
{
    aura_environment_block_system_signals();
    wl_display_run((struct wl_display*) data);
    return NULL;
}

//------------------------------------------------------------------------------

static void wayland_screen_refresh_handler(void* data)
{
    SurfaceId sid = (SurfaceId) data;
    LOG_WAYL3("Wayland: handling screen refresh");
    wayland_state_screen_refresh(sid);
}

//------------------------------------------------------------------------------

static void wayland_keyboard_focus_change_handler(void* data)
{
    SurfaceId sid = (SurfaceId) data;
    LOG_WAYL2("Wayland: handling keyboard focus change (%d)", sid);
    wayland_state_keyboard_focus_update(sid);
}

//------------------------------------------------------------------------------

static void wayland_keyboard_event_handler(void* data)
{
    LOG_WAYL4("Wayland: handling keyboard event");

    AuraKeyData* key_data = data;
    if (!key_data) {
        return;
    }

    wayland_state_key(key_data->time, key_data->code, key_data->value);
}

//------------------------------------------------------------------------------

int aura_wayland_event_loop_feeder(void* data)
{
    struct wl_event_loop* loop;
    static struct wl_event_source* src = NULL;

    LOG_WAYL4("--- Wayland loop feeder ---");
    loop = wl_display_get_event_loop(wayland_display);
    if (!src) {
        src = wl_event_loop_add_timer(loop,
                aura_wayland_event_loop_feeder, NULL);
    }
    wl_event_source_timer_update(src, 100);

    return 0;
}

//------------------------------------------------------------------------------

void aura_wayland_initialize(AuraLoop* this_loop)
{
    int result;
    pthread_t thread;

    LOG_INFO1("Initializing Wayland...");

    // Init Wayland
    wayland_display = wl_display_create();
    if (!wayland_display) {
        LOG_ERROR("Could not initialize Wafyland!");
        return;
    }

    wayland_state_initialize(wayland_display);

    // Create singleton objects // FIXME: not here
    if (!wl_global_create(wayland_display, &wl_compositor_interface, scVersion,
                          NULL, aura_wayland_compositor_bind)) {
        LOG_ERROR("Could not create global display!");
    }

    if (!wl_global_create(wayland_display, &wl_output_interface, scVersion,
                          NULL, aura_wayland_output_bind)) {
        LOG_ERROR("Could not create global output!");
    }

    if (!wl_global_create(wayland_display, &wl_shell_interface, 1,
                          NULL, aura_wayland_shell_bind)) {
        LOG_ERROR("Could not create global shell!");
    }

    if (!wl_global_create(wayland_display, &xdg_shell_interface, 1,
                          NULL, aura_wayland_xdg_shell_bind)) {
        LOG_ERROR("Could not create global XDG shell!");
    }

    if (!wl_global_create(wayland_display, &wl_seat_interface, scVersion,
                          NULL, aura_wayland_seat_bind)) {
        LOG_ERROR("Could not create global seat!");
    }

    wl_display_init_shm(wayland_display);

    // WORKAROUND:
    // Wayland main loop must be fed with some kind of epoll events,
    // otherwise it blocks. Here Wayland timer is used.
    aura_wayland_event_loop_feeder(NULL);

    // Add socket
    if (wl_display_add_socket(wayland_display, scSocketName)) {
        LOG_ERROR("Failed to add Wayland socket: %s", strerror(errno));
        return;
    }

    // Run wayland display in separate thread
    result = pthread_create(&thread, NULL, display_run, wayland_display);
    if (result != 0) {
        LOG_ERROR("Could not run Wayland display!");
        return;
    }

    // Subscribe for events
    aura_event_signal_subscribe(SIGNAL_SCREEN_REFRESH,
            aura_task_create(wayland_screen_refresh_handler, this_loop));

    aura_event_signal_subscribe(SIGNAL_KEYBOARD_FOCUS_CHANGED,
            aura_task_create(wayland_keyboard_focus_change_handler, this_loop));

    aura_event_signal_subscribe(SIGNAL_KEYBOARD_EVENT,
            aura_task_create(wayland_keyboard_event_handler, this_loop));

    LOG_INFO1("Initializing Wayland: SUCCESS");
}

//------------------------------------------------------------------------------

