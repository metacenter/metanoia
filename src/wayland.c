// file: wayland.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland.h"
#include "wayland-protocol-compositor.h"
#include "wayland-protocol-shell.h"
#include "wayland-protocol-seat.h"
#include "wayland-protocol-xdg-shell.h"
#include "wayland-protocol-output.h"
#include "wayland-protocol-screenshooter.h"
#include "wayland-cache.h"
#include "wayland-state.h"

#include "config.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "global-objects.h"
#include "event-signals.h"
#include "global-macros.h"

#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "xdg-shell-server-protocol.h"
#include "screenshooter-server-protocol.h"

//------------------------------------------------------------------------------

static pthread_t wayland_thread;
static struct wl_display* wayland_display;

//------------------------------------------------------------------------------

struct wl_display* get_wayland_display()
{
    return wayland_display;
}

//------------------------------------------------------------------------------

void* display_run(void* data)
{
    noia_environment_on_enter_new_thread(0, "noia:wayland");
    wl_display_run((struct wl_display*) data);
    return NULL;
}

//------------------------------------------------------------------------------

void wayland_screen_refresh_handler(void* data)
{
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) data);
    LOG_WAYL3("Wayland: handling screen refresh");
    noia_wayland_state_screen_refresh(sid);
}

//------------------------------------------------------------------------------

void wayland_keyboard_focus_change_handler(void* data)
{
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) data);
    LOG_WAYL2("Wayland: handling keyboard focus change (%d)", sid);
    noia_wayland_state_keyboard_focus_update(sid);
}

//------------------------------------------------------------------------------

void wayland_keyboard_event_handler(void* data)
{
    LOG_WAYL4("Wayland: handling keyboard event");

    NoiaKeyObject* object = (NoiaKeyObject*) data;
    if (!object) {
        return;
    }

    noia_wayland_state_key(object->keydata.time,
                           object->keydata.code,
                           object->keydata.value);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void wayland_pointer_focus_change_handler(void* data)
{
    LOG_WAYL4("Wayland: handling pointer focus change");
    NoiaMotionObject* object = (NoiaMotionObject*) data;
    noia_wayland_state_pointer_focus_update(object->sid, object->pos);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void wayland_pointer_motion_handler(void* data)
{
    LOG_WAYL4("Wayland: handling pointer motion");
    NoiaMotionObject* object = (NoiaMotionObject*) data;
    noia_wayland_state_pointer_motion(object->sid, object->pos);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void wayland_pointer_button_handler(void* data)
{
    LOG_WAYL4("Wayland: handling pointer button");
    NoiaButtonObject* object = (NoiaButtonObject*) data;
    noia_wayland_state_pointer_button(object->buttondata.time,
                                      object->buttondata.code,
                                      object->buttondata.value);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void wayland_output_found_handler(void* data)
{
    NoiaOutput* output = (NoiaOutput*) data;
    noia_wayland_state_advertise_output(output);
    noia_object_unref((NoiaObject*) output);
}

//------------------------------------------------------------------------------

void wayland_output_lost_handler(void* data)
{
    NoiaOutput* output = (NoiaOutput*) data;
    noia_wayland_state_destroy_output(output);
}

//------------------------------------------------------------------------------

void wayland_surface_reconfigured_handler(void* data)
{
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) data);
    noia_wayland_state_surface_reconfigured(sid);
}

//------------------------------------------------------------------------------

int noia_wayland_event_loop_feeder(void* data NOIA_UNUSED)
{
    struct wl_event_loop* loop;
    static struct wl_event_source* src = NULL;

    LOG_WAYL4("--- Wayland loop feeder ---");
    loop = wl_display_get_event_loop(wayland_display);
    if (!src) {
        src = wl_event_loop_add_timer(loop,
                noia_wayland_event_loop_feeder, NULL);
    }
    wl_event_source_timer_update(src, 100);

    return 0;
}

//------------------------------------------------------------------------------

void noia_wayland_finalize(void* data NOIA_UNUSED)
{
    noia_wayland_state_finalize();
    noia_wayland_cache_finalize();

    wl_display_terminate(wayland_display);
    LOG_INFO1("Wayland: waiting for thread to exit");
    pthread_join(wayland_thread, NULL);
    wl_display_destroy(wayland_display);
}

//------------------------------------------------------------------------------

void noia_wayland_initialize(NoiaLoop* this_loop)
{
    LOG_INFO1("Initializing Wayland...");

    // Init Wayland
    wayland_display = wl_display_create();
    if (!wayland_display) {
        LOG_ERROR("Could not initialize Wayland!");
        return;
    }

    noia_wayland_cache_initialize();
    noia_wayland_state_initialize(wayland_display);

    // Create singleton objects
    if (!wl_global_create(wayland_display, &wl_compositor_interface, 3,
                          NULL, noia_wayland_compositor_bind)) {
        LOG_ERROR("Could not create global display!");
    }

    if (!wl_global_create(wayland_display, &wl_shell_interface, 1,
                          NULL, noia_wayland_shell_bind)) {
        LOG_ERROR("Could not create global shell!");
    }

    if (!wl_global_create(wayland_display, &xdg_shell_interface, 1,
                          NULL, noia_wayland_xdg_shell_bind)) {
        LOG_ERROR("Could not create global XDG shell!");
    }

    if (!wl_global_create(wayland_display, &wl_seat_interface, 4,
                          NULL, noia_wayland_seat_bind)) {
        LOG_ERROR("Could not create global seat!");
    }

    if (!wl_global_create(wayland_display, &screenshooter_interface, 1,
                          NULL, noia_wayland_screenshooter_bind)) {
        LOG_ERROR("Could not create global screenshooter!");
    }

    wl_display_init_shm(wayland_display);

    /// @note WORKAROUND:
    /// Wayland main loop must be fed with some kind of epoll events,
    /// otherwise it blocks. Here Wayland timer is used.
    noia_wayland_event_loop_feeder(NULL);

    // Add socket
    if (wl_display_add_socket(wayland_display,
                              noia_settings()->wayland_display_name)) {
        LOG_ERROR("Failed to add Wayland socket '%s': %m",
                  noia_settings()->wayland_display_name);
        return;
    } else {
        LOG_WAYL1("Wayland socket name: '%s'",
                  noia_settings()->wayland_display_name);
    }

    // Run wayland display in separate thread
    if (pthread_create(&wayland_thread, NULL, display_run, wayland_display)) {
        LOG_ERROR("Could not run Wayland display!");
        return;
    }

    // Subscribe for events
    noia_event_signal_subscribe(SIGNAL_SCREEN_REFRESH,
               noia_task_create(wayland_screen_refresh_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_KEYBOARD_FOCUS_CHANGED,
               noia_task_create(wayland_keyboard_focus_change_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_KEYBOARD_EVENT,
               noia_task_create(wayland_keyboard_event_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_POINTER_FOCUS_CHANGED,
               noia_task_create(wayland_pointer_focus_change_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_POINTER_RELATIVE_MOTION,
               noia_task_create(wayland_pointer_motion_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_POINTER_BUTTON,
               noia_task_create(wayland_pointer_button_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_OUTPUT_FOUND,
               noia_task_create(wayland_output_found_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_OUTPUT_LOST,
               noia_task_create(wayland_output_lost_handler,
                                this_loop, NULL));

    noia_event_signal_subscribe(SIGNAL_SURFACE_RECONFIGURED,
               noia_task_create(wayland_surface_reconfigured_handler,
                                this_loop, NULL));

    noia_loop_add_finalizer(this_loop, noia_wayland_finalize);

    LOG_INFO1("Initializing Wayland: SUCCESS");
}

//------------------------------------------------------------------------------

