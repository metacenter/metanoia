// file: wayland-state.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-state.h"
#include "wayland-output.h"
#include "wayland-cache.h"
#include "wayland-protocol-output.h"

#include "surface-manager.h"
#include "utils-log.h"

#include "xdg-shell-server-protocol.h"

#include <malloc.h>
#include <memory.h>
#include <pthread.h>

pthread_mutex_t sStateMutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

static struct {
    struct wl_display* display;
    AuraStore* outputs;
    AuraSurfaceId keyboard_focused_sid;
} sState;

//------------------------------------------------------------------------------

struct wl_resource* aura_wayland_state_get_rc_for_sid(AuraSurfaceId sid)
{
    AuraWaylandSurface* surface = aura_wayland_cache_find_surface(sid);
    return aura_wayland_surface_get_resource(surface, AURA_RESOURCE_SURFACE);
}

//------------------------------------------------------------------------------

AuraResult aura_wayland_state_initialize(struct wl_display* display)
{
    sState.outputs = aura_store_new_for_str();
    if (!sState.outputs) {
        return AURA_RESULT_ERROR;
    }

    sState.display = display;
    sState.keyboard_focused_sid = scInvalidItemId;

    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void aura_wayland_state_finalize()
{
    sState.keyboard_focused_sid = scInvalidItemId;
    sState.display = NULL;

    if (sState.outputs) {
        aura_store_free_with_items(sState.outputs,
                                  (AuraFreeFunc) aura_wayland_output_destroy);
        sState.outputs = NULL;
    }
}

//------------------------------------------------------------------------------

void aura_wayland_state_add_surface(AuraSurfaceId sid, struct wl_resource* rc)
{
    pthread_mutex_lock(&sStateMutex);

    LOG_INFO2("Wayland: adding surface (sid: %d)", sid);
    AuraWaylandSurface* surface = aura_wayland_surface_new();
    aura_wayland_cache_add_surface(sid, surface);
    aura_wayland_cache_add_surface_resource(sid, AURA_RESOURCE_SURFACE, rc);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_remove_surface(AuraSurfaceId sid)
{
    pthread_mutex_lock(&sStateMutex);

    LOG_INFO2("Wayland: removing surface (sid: %d)", sid);
    aura_wayland_cache_remove_surface(sid);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_surface_attach(AuraSurfaceId sid,
                                       struct wl_resource* rc)
{
    pthread_mutex_lock(&sStateMutex);

    AuraWaylandSurface* surface = aura_wayland_cache_find_surface(sid);
    aura_wayland_surface_set_resource(surface, AURA_RESOURCE_BUFFER, rc);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_subscribe_frame(AuraSurfaceId sid,
                                        struct wl_resource* rc)
{
    pthread_mutex_lock(&sStateMutex);

    AuraWaylandSurface* surface = aura_wayland_cache_find_surface(sid);
    aura_wayland_surface_set_resource(surface, AURA_RESOURCE_FRAME, rc);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_add_keyboard_resource(struct wl_resource* rc)
{
    struct wl_resource* surface_rc = NULL;
    struct wl_client* focused_client = NULL;

    pthread_mutex_lock(&sStateMutex);

    LOG_WAYL2("Wayland: adding keyboard resource");

    // Store new resource
    aura_wayland_cache_add_general_resource(AURA_RESOURCE_KEYBOARD, rc);

    // If client is focused, send enter event
    surface_rc = aura_wayland_state_get_rc_for_sid(sState.keyboard_focused_sid);
    if (surface_rc) {
        focused_client = wl_resource_get_client(surface_rc);
    }

    struct wl_client* rc_client = wl_resource_get_client(rc);
    if (focused_client && rc_client && (rc_client == focused_client)) {
        struct wl_array array;
        wl_array_init(&array);
        int serial = wl_display_next_serial(sState.display);
        wl_keyboard_send_enter(rc, serial, surface_rc, &array);
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_keyboard_focus_update(AuraSurfaceId new_sid)
{
    Chain* resources = NULL;
    struct wl_resource* new_resource = NULL;
    struct wl_client* new_client = NULL;
    struct wl_resource* old_resource = NULL;
    struct wl_client* old_client = NULL;

    pthread_mutex_lock(&sStateMutex);

    AuraSurfaceId old_sid = sState.keyboard_focused_sid;
    LOG_WAYL2("Wayland: keyboard focus update (oldsid: %u, newsid: %u)",
              old_sid, new_sid);

    new_resource = aura_wayland_state_get_rc_for_sid(new_sid);
    if (new_resource) {
        new_client = wl_resource_get_client(new_resource);
    }

    old_resource = aura_wayland_state_get_rc_for_sid(old_sid);
    if (old_resource) {
        old_client = wl_resource_get_client(old_resource);
    }

    // Check if new and old clients are different
    if (new_client != old_client) {
        // Clear current client
        sState.keyboard_focused_sid = scInvalidItemId;

        // Send 'leave' and 'enter' event to all clients' keyboard objects
        struct wl_array array;
        wl_array_init(&array);
        resources = aura_wayland_cache_get_resources(AURA_RESOURCE_KEYBOARD);
        for (Link* link = resources->first; link; link = link->next) {
            struct wl_resource* rc = link->data;
            struct wl_client* rc_client = wl_resource_get_client(rc);

            if (rc_client == old_client) {
                /// @todo: wl_keyboard_send_leave
            }
            if (rc_client == new_client) {
                wl_keyboard_send_enter(rc, 0, new_resource, &array);
            }
        }

        // Update current client
        sState.keyboard_focused_sid = new_sid;
    }
    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_key(uint32_t time, uint32_t key, uint32_t state)
{
    struct wl_resource* resource = NULL;
    struct wl_client* focused_client = NULL;
    Chain* resources = NULL;

    pthread_mutex_lock(&sStateMutex);

    if (sState.keyboard_focused_sid == scInvalidItemId) {
        pthread_mutex_unlock(&sStateMutex);
        return;
    }

    LOG_WAYL4("Wayland: key (sid: %u, time: %u, key: %u, state: %u)",
              sState.keyboard_focused_sid, time, key, state);

    resource = aura_wayland_state_get_rc_for_sid(sState.keyboard_focused_sid);
    if (resource) {
        focused_client = wl_resource_get_client(resource);
    }

    if (resource && focused_client) {
        int serial = wl_display_next_serial(sState.display);
        resources = aura_wayland_cache_get_resources(AURA_RESOURCE_KEYBOARD);
        for (Link* link = resources->first; link; link = link->next) {
            struct wl_resource* rc = link->data;
            if (focused_client == wl_resource_get_client(rc)) {
                wl_keyboard_send_key(rc, serial, time, key, state);
            }
        }
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_screen_refresh(AuraSurfaceId sid)
{
    pthread_mutex_lock(&sStateMutex);

    LOG_WAYL3("Wayland: screen refresh (sid: %u)", sid);

    AuraWaylandSurface* surface = aura_wayland_cache_find_surface(sid);

    // Release buffer resource
    struct wl_resource* buffer_rc =
               aura_wayland_surface_get_resource(surface, AURA_RESOURCE_BUFFER);
    if (buffer_rc) {
        wl_resource_queue_event(buffer_rc, WL_BUFFER_RELEASE);
    }

    // Notify frame and release frame resource
    struct wl_resource* frame_rc =
                aura_wayland_surface_get_resource(surface, AURA_RESOURCE_FRAME);
    if (frame_rc) {
        LOG_WAYL4("Wayland: Sending frame (id: %u)", sid);

        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint32_t msec = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

        wl_callback_send_done(frame_rc, msec);
        wl_resource_destroy(frame_rc);
        aura_wayland_surface_set_resource(surface, AURA_RESOURCE_FRAME, NULL);
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_advertise_output(AuraOutput* output)
{
    pthread_mutex_lock(&sStateMutex);

    struct wl_global* global = wl_global_create(sState.display,
                     &wl_output_interface, 2, output, aura_wayland_output_bind);

    if (!global) {
        LOG_ERROR("Could not create global output!");
    }

    AuraWaylandOutput* wayland_output =
                                     aura_wayland_output_create(global, output);
    aura_store_add(sState.outputs, output->unique_name, wayland_output);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_destroy_output(AuraOutput* output)
{
    pthread_mutex_lock(&sStateMutex);

    AuraWaylandOutput* wayland_output =
                         aura_store_delete(sState.outputs, output->unique_name);
    wl_global_destroy(wayland_output->global_output);
    aura_wayland_output_destroy(wayland_output);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_state_surface_reconfigured(AuraSurfaceId sid)
{
    pthread_mutex_lock(&sStateMutex);

    AuraSurfaceData* data = NULL;
    AuraWaylandSurface* surface = aura_wayland_cache_find_surface(sid);
    if (surface) {
        data = aura_surface_get(sid);
    }

    if (surface && data) {
        LOG_DEBUG("Wayland: surface reconfiguration "
                  "(sid: %d, width: %d, height: %d)",
                   sid, data->desired_size.width, data->desired_size.height);

        struct wl_resource* shell_surface_rc =
                            aura_wayland_surface_get_resource
                                         (surface, AURA_RESOURCE_SHELL_SURFACE);
        struct wl_resource* xdg_shell_surface_rc =
                            aura_wayland_surface_get_resource
                                     (surface, AURA_RESOURCE_XDG_SHELL_SURFACE);

        if (shell_surface_rc) {
            /// @todo Implement resizing for Wayland shell
        } else if (xdg_shell_surface_rc) {
            struct wl_array states;
            wl_array_init(&states);
            int serial = wl_display_next_serial(sState.display);
            xdg_surface_send_configure(xdg_shell_surface_rc,
                                       data->desired_size.width,
                                       data->desired_size.height,
                                       &states, serial);
            wl_array_release(&states);
        }
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

