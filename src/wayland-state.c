// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "wayland-state.h"
#include "wayland-output.h"
#include "wayland-cache.h"
#include "wayland-protocol-output.h"

#include "surface-coordinator.h"
#include "utils-keyboard-state.h"
#include "utils-log.h"

#include "xdg-shell-server-protocol.h"

#include <malloc.h>
#include <memory.h>
#include <pthread.h>

pthread_mutex_t sStateMutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

static struct {
    struct wl_display* display;
    NoiaCoordinator* coordinator;
    NoiaStore* outputs;
    NoiaKeyboardState* keyboard_state;
    NoiaSurfaceId keyboard_focused_sid;
    NoiaSurfaceId pointer_focused_sid;
    int pointer_serial;
} sState;

//------------------------------------------------------------------------------

struct wl_resource* noia_wayland_state_get_rc_for_sid(NoiaSurfaceId sid)
{
    struct wl_resource* result = NULL;
    NoiaWaylandSurface* surface = noia_wayland_cache_find_surface(sid);
    if (surface) {
        result = noia_wayland_surface_get_resource(surface,
                                                   NOIA_RESOURCE_SURFACE);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_wayland_state_initialize(struct wl_display* display,
                                         NoiaCoordinator* coordinator)
{
    sState.outputs = noia_store_new_for_str();
    sState.keyboard_state = noia_keyboard_state_new();
    noia_keyboard_state_initialize(sState.keyboard_state);

    sState.display = display;
    sState.coordinator = coordinator;
    sState.keyboard_focused_sid = scInvalidItemId;
    sState.pointer_focused_sid = scInvalidItemId;
    sState.pointer_serial = 0;

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_wayland_state_finalize(void)
{
    sState.pointer_serial = 0;
    sState.pointer_focused_sid = scInvalidItemId;
    sState.keyboard_focused_sid = scInvalidItemId;
    sState.display = NULL;

    noia_keyboard_state_finalize(sState.keyboard_state);
    noia_keyboard_state_free(sState.keyboard_state);
    sState.keyboard_state = NULL;

    if (sState.outputs) {
        noia_store_free_with_items(sState.outputs,
                                  (NoiaFreeFunc) noia_wayland_output_destroy);
        sState.outputs = NULL;
    }
}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_wayland_state_create_surface()
{
    return noia_surface_create(sState.coordinator);
}

//------------------------------------------------------------------------------

void noia_wayland_state_add_surface(NoiaSurfaceId sid, struct wl_resource* rc)
{
    pthread_mutex_lock(&sStateMutex);

    LOG_WAYL1("Wayland: adding surface (sid: %d)", sid);
    NoiaWaylandSurface* surface = noia_wayland_surface_new();
    noia_wayland_cache_add_surface(sid, surface);
    noia_wayland_cache_add_surface_resource(sid, NOIA_RESOURCE_SURFACE, rc);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_add_shell_surface(NoiaSurfaceId sid,
                                          NoiaWaylandSurfaceResourceType type,
                                          struct wl_resource* rc)
{
    pthread_mutex_lock(&sStateMutex);

    noia_wayland_cache_add_surface_resource(sid, type, rc);
    noia_surface_show(sState.coordinator, sid, NOIA_SURFACE_SHOW_IN_SHELL);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_remove_surface(NoiaSurfaceId sid,
                                       struct wl_resource* rc)
{
    pthread_mutex_lock(&sStateMutex);

    LOG_WAYL1("Wayland: removing surface (sid: %d)", sid);
    noia_surface_destroy(sState.coordinator, sid);
    noia_wayland_cache_remove_surface_resource(sid, NOIA_RESOURCE_SURFACE, rc);
    noia_wayland_cache_remove_surface(sid);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_surface_attach(NoiaSurfaceId sid,
                                       struct wl_resource* rc,
                                       struct wl_resource* brc,
                                       int width,
                                       int height,
                                       int stride,
                                       uint8_t* data)
{
    pthread_mutex_lock(&sStateMutex);

    NoiaWaylandSurface* surface = noia_wayland_cache_find_surface(sid);
    noia_wayland_surface_add_resource(surface, NOIA_RESOURCE_BUFFER, brc);
    noia_surface_attach(sState.coordinator, sid,
                        width, height, stride, data, rc);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_surface_commit(NoiaItemId sid)
{
    noia_surface_commit(sState.coordinator, sid);
}

//------------------------------------------------------------------------------

void noia_wayland_state_surface_set_offset(NoiaItemId sid, NoiaPosition pos)
{
    noia_surface_set_offset(sState.coordinator, sid, pos);
}

//------------------------------------------------------------------------------

void noia_wayland_state_surface_set_requested_size(NoiaItemId sid,
                                                   NoiaSize size)
{
    noia_surface_set_requested_size(sState.coordinator, sid, size);
}

//------------------------------------------------------------------------------

void noia_wayland_state_subscribe_frame(NoiaSurfaceId sid,
                                        struct wl_resource* rc)
{
    pthread_mutex_lock(&sStateMutex);

    NoiaWaylandSurface* surface = noia_wayland_cache_find_surface(sid);
    noia_wayland_surface_add_resource(surface, NOIA_RESOURCE_FRAME, rc);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_add_keyboard_resource(struct wl_resource* rc)
{
    struct wl_resource* surface_rc = NULL;
    struct wl_client* focused_client = NULL;

    pthread_mutex_lock(&sStateMutex);

    LOG_WAYL2("Wayland: adding keyboard resource");

    // Store new resource
    noia_wayland_cache_add_general_resource(NOIA_RESOURCE_KEYBOARD, rc);

    // If client is focused, send enter event
    surface_rc = noia_wayland_state_get_rc_for_sid(sState.keyboard_focused_sid);
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

void noia_wayland_state_keyboard_focus_update(NoiaSurfaceId new_sid)
{
    struct wl_resource* new_resource = NULL;
    struct wl_client* new_client = NULL;
    struct wl_resource* old_resource = NULL;
    struct wl_client* old_client = NULL;

    pthread_mutex_lock(&sStateMutex);

    NoiaSurfaceId old_sid = sState.keyboard_focused_sid;
    LOG_WAYL2("Wayland < keyboard focus update (oldsid: %u, newsid: %u)",
              old_sid, new_sid);

    new_resource = noia_wayland_state_get_rc_for_sid(new_sid);
    if (new_resource) {
        new_client = wl_resource_get_client(new_resource);
    }

    old_resource = noia_wayland_state_get_rc_for_sid(old_sid);
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
        NoiaList* resources =
                       noia_wayland_cache_get_resources(NOIA_RESOURCE_KEYBOARD);
        FOR_EACH (resources, link) {
            struct wl_resource* rc = link->data;
            struct wl_client* rc_client = wl_resource_get_client(rc);

            if (rc_client == old_client) {
                wl_keyboard_send_leave(rc, 0, old_resource);
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

void noia_wayland_state_key(uint32_t time, uint32_t key, uint32_t state)
{
    struct wl_resource* resource = NULL;
    struct wl_client* focused_client = NULL;

    pthread_mutex_lock(&sStateMutex);

    // Update keyboard state
    NoiaKeyMods old_mods =
                       noia_keyboard_state_get_modifiers(sState.keyboard_state);
    noia_keyboard_state_update_key(sState.keyboard_state, key, state);

    // Check if there is someone to be notified
    if (sState.keyboard_focused_sid == scInvalidItemId) {
        pthread_mutex_unlock(&sStateMutex);
        return;
    }

    LOG_WAYL4("Wayland < key (sid: %u, time: %u, key: %u, state: %u)",
              sState.keyboard_focused_sid, time, key, state);

    resource = noia_wayland_state_get_rc_for_sid(sState.keyboard_focused_sid);
    if (resource) {
        focused_client = wl_resource_get_client(resource);
    }

    // Notify the client
    if (resource and focused_client) {
        int serial = wl_display_next_serial(sState.display);
        NoiaList* resources =
                       noia_wayland_cache_get_resources(NOIA_RESOURCE_KEYBOARD);
        FOR_EACH (resources, link) {
            struct wl_resource* rc = link->data;
            if (focused_client == wl_resource_get_client(rc)) {
                // Send key
                wl_keyboard_send_key(rc, serial, time, key, state);

                // Send modifiers
                NoiaKeyMods new_mods =
                       noia_keyboard_state_get_modifiers(sState.keyboard_state);
                if (not noia_keymods_are_equal(&new_mods, &old_mods)) {
                    wl_keyboard_send_modifiers(rc, serial,
                                               new_mods.depressed,
                                               new_mods.latched,
                                               new_mods.locked,
                                               new_mods.effective);
                }
            }
        }
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_pointer_focus_update(NoiaSurfaceId new_sid,
                                             NoiaPosition pos)
{
    NoiaList* resources = NULL;
    struct wl_resource* new_resource = NULL;
    struct wl_client* new_client = NULL;
    struct wl_resource* old_resource = NULL;
    struct wl_client* old_client = NULL;

    pthread_mutex_lock(&sStateMutex);

    NoiaSurfaceId old_sid = sState.pointer_focused_sid;

    new_resource = noia_wayland_state_get_rc_for_sid(new_sid);
    if (new_resource) {
        new_client = wl_resource_get_client(new_resource);
    }

    old_resource = noia_wayland_state_get_rc_for_sid(old_sid);
    if (old_resource) {
        old_client = wl_resource_get_client(old_resource);
    }

    sState.pointer_serial = wl_display_next_serial(sState.display);
    resources = noia_wayland_cache_get_resources(NOIA_RESOURCE_POINTER);

    FOR_EACH (resources, link) {
        struct wl_resource* rc = link->data;
        if (old_client == wl_resource_get_client(rc)) {
            wl_pointer_send_leave(rc, sState.pointer_serial, old_resource);
        }
    }

    sState.pointer_focused_sid = new_sid;

    FOR_EACH (resources, link) {
        struct wl_resource* rc = link->data;
        if (new_client == wl_resource_get_client(rc)) {
            wl_pointer_send_enter(rc, sState.pointer_serial, new_resource,
                            wl_fixed_from_int(pos.x), wl_fixed_from_int(pos.y));
        }
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_pointer_motion(NoiaSurfaceId sid, NoiaPosition pos)
{
    NoiaList* resources = NULL;
    struct wl_resource* resource = NULL;
    struct wl_client* client = NULL;

    pthread_mutex_lock(&sStateMutex);

    resource = noia_wayland_state_get_rc_for_sid(sid);
    if (resource) {
        client = wl_resource_get_client(resource);
    }

    resources = noia_wayland_cache_get_resources(NOIA_RESOURCE_POINTER);

    FOR_EACH (resources, link) {
        struct wl_resource* rc = link->data;
        if (client == wl_resource_get_client(rc)) {
            wl_pointer_send_motion(rc, (int32_t) noia_log_get_miliseconds(),
                            wl_fixed_from_int(pos.x), wl_fixed_from_int(pos.y));
        }
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_pointer_button(uint32_t time,
                                       uint32_t button,
                                       uint32_t state)
{
    NoiaList* resources = NULL;
    struct wl_resource* resource = NULL;
    struct wl_client* client = NULL;

    pthread_mutex_lock(&sStateMutex);

    resource = noia_wayland_state_get_rc_for_sid(sState.pointer_focused_sid);
    if (resource) {
        client = wl_resource_get_client(resource);
    }

    resources = noia_wayland_cache_get_resources(NOIA_RESOURCE_POINTER);

    FOR_EACH (resources, link) {
        struct wl_resource* rc = link->data;
        if (client == wl_resource_get_client(rc)) {
            int serial = wl_display_next_serial(sState.display);
            wl_pointer_send_button(rc, serial, time, button, state);
        }
    }

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_set_cursor(int serial,
                                   int hotspot_x,
                                   int hotspot_y,
                                   NoiaSurfaceId sid)
{
    if (serial != sState.pointer_serial) {
        LOG_WARN2("Unauthorized set of cursor "
                  "(received serial: %d, sent serial: %d)",
                  serial, sState.pointer_serial);
        return;
    }

    NoiaPosition hotspot = {hotspot_x, hotspot_y};
    noia_surface_set_offset(sState.coordinator, sid, hotspot);
    noia_surface_set_as_cursor(sid);
}

//------------------------------------------------------------------------------

void noia_wayland_state_screen_refresh(NoiaSurfaceId sid)
{
    pthread_mutex_lock(&sStateMutex);

    LOG_WAYL4("Wayland: screen refresh (sid: %u)", sid);

    NoiaWaylandSurface* surface = noia_wayland_cache_find_surface(sid);
    const NoiaList* frcs = noia_wayland_surface_get_frame_resources(surface);

    // Release buffer if needed
    if (noia_list_len(frcs) > 0) {
        struct wl_resource* buffer_rc =
               noia_wayland_surface_get_resource(surface, NOIA_RESOURCE_BUFFER);
        if (buffer_rc) {
            wl_resource_queue_event(buffer_rc, WL_BUFFER_RELEASE);
        }
    }

    // Notify frame
    FOR_EACH(frcs, link) {
        struct wl_resource* frame_rc = (struct wl_resource*) link->data;
        LOG_WAYL3("Wayland < frame (sid: %u)", sid);
        wl_callback_send_done(frame_rc, (uint32_t) noia_log_get_miliseconds());
    }
    noia_wayland_surface_remove_frame_resources(surface);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_advertise_output(NoiaOutput* output)
{
    pthread_mutex_lock(&sStateMutex);

    struct wl_global* global = wl_global_create(sState.display,
                     &wl_output_interface, 2, output, noia_wayland_output_bind);

    if (!global) {
        LOG_ERROR("Could not create global output!");
    }

    NoiaWaylandOutput* wayland_output =
                                     noia_wayland_output_create(global, output);
    noia_store_add(sState.outputs, output->unique_name, wayland_output);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_destroy_output(NoiaOutput* output)
{
    pthread_mutex_lock(&sStateMutex);

    NoiaWaylandOutput* wayland_output =
                         noia_store_delete(sState.outputs, output->unique_name);
    wl_global_destroy(wayland_output->global_output);
    noia_wayland_output_destroy(wayland_output);

    pthread_mutex_unlock(&sStateMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_state_surface_reconfigured(NoiaSurfaceId sid)
{
    pthread_mutex_lock(&sStateMutex);

    NoiaSurfaceData* data = NULL;
    NoiaWaylandSurface* surface = noia_wayland_cache_find_surface(sid);
    if (surface) {
        data = noia_surface_get(sState.coordinator, sid);
    }

    if (surface and data) {
        LOG_INFO2("Wayland < surface reconfiguration "
                  "(sid: %d, width: %d, height: %d)",
                   sid, data->desired_size.width, data->desired_size.height);

        struct wl_resource* shell_surface_rc =
                            noia_wayland_surface_get_resource
                                         (surface, NOIA_RESOURCE_SHELL_SURFACE);
        struct wl_resource* xdg_shell_surface_rc =
                            noia_wayland_surface_get_resource
                                     (surface, NOIA_RESOURCE_XDG_SHELL_SURFACE);

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

