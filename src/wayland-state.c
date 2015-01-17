// file: wayland-state.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-state.h"
#include "utils-chain.h"
#include "utils-store.h"
#include "utils-log.h"

#include "malloc.h"
#include "memory.h"
#include "pthread.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

static struct {
    struct wl_display* display;
    AuraStore* surfaces;
    Chain* keyboard_resources;
    AuraItemId keyboard_focused_sid;
} sState;


// FIXME: tmp
uint32_t serial = 0;

//------------------------------------------------------------------------------

AuraSurfaceWaylandData* wayland_surface_data_new()
{
    AuraSurfaceWaylandData* data = malloc(sizeof(AuraSurfaceWaylandData));
    if (!data) {
        return 0;
    }

    memset(data, 0, sizeof(AuraSurfaceWaylandData));
    return data;
}

//------------------------------------------------------------------------------

int wayland_state_initialize(struct wl_display* display)
{
    sState.surfaces = aura_store_new_for_id();
    if (!sState.surfaces) {
        return -1;
    }

    sState.keyboard_resources = chain_new(NULL);
    if (!sState.keyboard_resources) {
        return -1;
    }

    sState.display = display;
    sState.keyboard_focused_sid = scInvalidItemId;

    return 0;
}

//------------------------------------------------------------------------------

void wayland_state_finalize()
{
    // TODO
}

//------------------------------------------------------------------------------

void wayland_state_add_surface(AuraItemId sid, struct wl_resource* rc)
{
    pthread_mutex_lock(&mutex);

    LOG_INFO2("Wayland: adding surface (sid: %d)", sid);
    AuraSurfaceWaylandData* surface_data = wayland_surface_data_new();
    surface_data->base.id = sid;
    surface_data->resource = rc;

    aura_store_add(sState.surfaces, sid, surface_data);

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_remove_surface(AuraItemId sid)
{
    pthread_mutex_lock(&mutex);

    LOG_INFO2("Wayland: removing surface (sid: %d)", sid);

    aura_store_delete(sState.surfaces, sid);

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_surface_attach(AuraItemId sid, struct wl_resource* rc)
{
    pthread_mutex_lock(&mutex);

    AuraSurfaceWaylandData* data = aura_store_get(sState.surfaces, sid);
    if (!data) {
        // This is not a Wayland surface
        LOG_INFO3("SID %d does not resolve to any surface", sid);
        pthread_mutex_unlock(&mutex);
        return;
    }

    data->buffer_resource = rc;

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_subscribe_frame(AuraItemId sid, struct wl_resource* rc)
{
    pthread_mutex_lock(&mutex);

    AuraSurfaceWaylandData* data = aura_store_get(sState.surfaces, sid);
    if (!data) {
        // This is not a Wayland surface
        LOG_INFO3("SID %d does not resolve to any surface", sid);
        pthread_mutex_unlock(&mutex);
        return;
    }

    data->frame_resource = rc;

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_add_keyboard_resource(struct wl_resource* keyboard_rc)
{
    pthread_mutex_lock(&mutex);

    LOG_WAYL2("Wayland: adding keyboard resource");

    // Store new resource
    chain_append(sState.keyboard_resources, keyboard_rc);

    // TODO: If client is focused, sent enter event
    AuraSurfaceWaylandData* data =
                   aura_store_get(sState.surfaces, sState.keyboard_focused_sid);
    if (!data) {
        // This is not a Wayland surface
        LOG_WAYL5("New SID does not resolve to any surface");
        pthread_mutex_unlock(&mutex);
        return;
    }

    struct wl_client* focused_client = wl_resource_get_client(data->resource);
    struct wl_client* new_client = wl_resource_get_client(keyboard_rc);
    if (new_client == focused_client) {
        struct wl_array array;
        wl_array_init(&array);
        serial = wl_display_next_serial(sState.display);
        wl_keyboard_send_enter(keyboard_rc, serial, data->resource, &array);
    }

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_keyboard_focus_update(AuraItemId new_sid)
{
    pthread_mutex_lock(&mutex);

    AuraItemId old_sid = sState.keyboard_focused_sid;
    LOG_WAYL2("Wayland: keyboard focus update (oldsid: %u, newsid: %u)",
              old_sid, new_sid);

    AuraSurfaceWaylandData* new_data = aura_store_get(sState.surfaces, new_sid);
    if (!new_data) {
        // This is not a Wayland surface
        LOG_WAYL5("New SID does not resolve to any surface");
        sState.keyboard_focused_sid = scInvalidItemId;
        pthread_mutex_unlock(&mutex);
        return;
    }
    struct wl_client* new_client = wl_resource_get_client(new_data->resource);

    AuraSurfaceWaylandData* old_data = NULL;
    struct wl_client* old_client = NULL;
    if (old_sid != scInvalidItemId) {
        old_data = aura_store_get(sState.surfaces, old_sid);
        old_client = wl_resource_get_client(old_data->resource);
    }

    // Check if new and old clients are different
    if (new_client == old_client) {
        sState.keyboard_focused_sid = new_sid;
        pthread_mutex_unlock(&mutex);
        return;
    }

    // Clear current client
    sState.keyboard_focused_sid = scInvalidItemId;

    // Send 'leave' and 'enter' event to all clients' keyboard objects
    struct wl_array array;
    wl_array_init(&array);
    Link* link = sState.keyboard_resources->first;
    while (link) {
        struct wl_resource* rc = link->data;
        struct wl_client* rc_client = wl_resource_get_client(rc);

        if (rc_client == old_client) {
            // TODO: wl_keyboard_send_leave
        }
        if (rc_client == new_client) {
            wl_keyboard_send_enter(rc, 0, new_data->resource, &array);
        }

        link = link->next;
    }

    // Update current client
    sState.keyboard_focused_sid = new_sid;

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_key(uint32_t time, uint32_t key, uint32_t state)
{
    pthread_mutex_lock(&mutex);

    if (sState.keyboard_focused_sid == scInvalidItemId) {
        pthread_mutex_unlock(&mutex);
        return;
    }

    LOG_WAYL4("Wayland: key (sid: %u, time: %u, key: %u, state: %u)",
              sState.keyboard_focused_sid, time, key, state);

    AuraSurfaceWaylandData* data = aura_store_get(sState.surfaces,
                                                  sState.keyboard_focused_sid);
    if (!data) {
        // This is not a Wayland surface
        LOG_WAYL5("Given SID does not resolve to any surface");
        pthread_mutex_unlock(&mutex);
        return;
    }

    serial = wl_display_next_serial(sState.display);
    Link* link = sState.keyboard_resources->first;
    while (link) {
        struct wl_resource* rc = link->data;
        wl_keyboard_send_key(rc, serial, time, key, state);
        link = link->next;
    }

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_screen_refresh(AuraItemId sid)
{
    pthread_mutex_lock(&mutex);

    LOG_WAYL3("Wayland: screen refresh (sid: %u)", sid);

    AuraSurfaceWaylandData* data = aura_store_get(sState.surfaces, sid);
    if (!data) {
        // This is not a Wayland surface
        LOG_WAYL5("SID %d does not resolve to any surface", sid);
        pthread_mutex_unlock(&mutex);
        return;
    }

    // Release buffer resource
    if (data->buffer_resource) {
        wl_resource_queue_event(data->buffer_resource, WL_BUFFER_RELEASE);
    }

    // Notify frame and release frame resource
    if (data->frame_resource) {
        LOG_WAYL4("Wayland: Sending frame (id: %u)", sid);

        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint32_t msec = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

        wl_callback_send_done(data->frame_resource, msec);
        wl_resource_destroy(data->frame_resource);
        data->frame_resource = NULL;
    }

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

