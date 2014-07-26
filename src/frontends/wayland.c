// file: wayland.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "frontends/wayland.h"
#include "frontends/wayland-compositor.h"
#include "frontends/wayland-shell.h"
#include "frontends/wayland-xdg_shell.h"
#include "frontends/wayland-output.h"

#include "utils/log.h"

#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <wayland-server.h>

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

static const int scVersion = 2;
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

void aura_wayland_initialize(void)
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

    LOG_INFO1("Initializing Wayland: SUCCESS");
}

//------------------------------------------------------------------------------

