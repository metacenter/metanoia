// file: wayland.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland.h"
#include "wayland-compositor.h"
#include "wayland-shell.h"
#include "wayland-xdg_shell.h"
#include "wayland-output.h"

#include "utils-log.h"
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
    wl_display_run((struct wl_display*) data);
    return NULL;
}

//------------------------------------------------------------------------------

static void wayland_keyboard_focus_change_handler(void* data)
{
    LOG_INFO1("SIGNAL HANDLED!");
}

//------------------------------------------------------------------------------

int aura_wayland_event_loop_feeder(void* data)
{
    struct wl_event_loop* loop;
    struct wl_event_source* src;

    loop = wl_display_get_event_loop(wayland_display);
    src = wl_event_loop_add_timer(loop, aura_wayland_event_loop_feeder, NULL);
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

    LOG_DEBUG("WLD: %p", wayland_display);

    // Create singleton objects
    if (!wl_global_create(wayland_display, &wl_compositor_interface, scVersion,
                          NULL, aura_wayland_compositor_bind)) {
        LOG_ERROR("Could not create global display!");
    }

    // FIXME: not here
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
    aura_event_signal_subscribe(SIGNAL_KEYBOARD_FOCUS_CHANGED,
            aura_task_create(wayland_keyboard_focus_change_handler, this_loop));

    LOG_INFO1("Initializing Wayland: SUCCESS");
}

//------------------------------------------------------------------------------

void aura_wayland_notify_frame(SurfaceData* surface)
{
    if (!surface) {
        LOG_ERROR("Got null surface!");
        return;
    }

    LOG_INFO3("Wayland: Notify frame (id: %d)", surface->id);

    void* notify_data = surface->frame_notify_data;
    void* buffer_resource = surface->buffer_resource;

    if (buffer_resource) {
        wl_resource_queue_event(buffer_resource, WL_BUFFER_RELEASE);
    }

    if (notify_data) {
        // TODO: pass time as argument
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint32_t msec = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

        wl_callback_send_done(notify_data, msec);
        wl_resource_destroy(notify_data);
    }

    surface->frame_notify_data = NULL;
}

//------------------------------------------------------------------------------

