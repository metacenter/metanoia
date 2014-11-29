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
    AuraStore* surfaces;
    Chain* keyboard_resources;
    struct wl_client* keyboard_focused_client;
} sState;

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

int wayland_state_initialize()
{
    sState.surfaces = aura_store_new();
    if (!sState.surfaces) {
        return -1;
    }

    sState.keyboard_resources = chain_new(NULL);
    if (!sState.keyboard_resources) {
        return -1;
    }

    sState.keyboard_focused_client = NULL;

    return 0;
}

//------------------------------------------------------------------------------

void wayland_state_finalize()
{
    // TODO
}

//------------------------------------------------------------------------------

void wayland_state_add_surface(AuraItemId sid, struct wl_resource* resource)
{
    pthread_mutex_lock(&mutex);

    LOG_INFO3("Wayland: adding surface (sid: %d)", sid);
    AuraSurfaceWaylandData* surface_data = wayland_surface_data_new();
    surface_data->base.id = sid;
    surface_data->resource = resource;

    aura_store_add(sState.surfaces, sid, surface_data);

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void wayland_state_add_keyboard_resource(struct wl_resource* keyboard_resource,
                                         struct wl_resource* surface_resource)
{
    pthread_mutex_lock(&mutex);

    LOG_INFO2("Wayland: adding keyboard resource");

    // Store new resource
    chain_append(sState.keyboard_resources, keyboard_resource);

    // If client is focused, sent enter event
    struct wl_client* new_client = wl_resource_get_client(keyboard_resource);
    if (new_client == sState.keyboard_focused_client) {
        struct wl_array array;
        wl_array_init(&array);
        wl_keyboard_send_enter(keyboard_resource, 0, surface_resource, &array);
    }

    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

int wayland_state_keyboard_focus_update(AuraItemId sid)
{
    pthread_mutex_lock(&mutex);

    LOG_INFO2("Wayland: keyboard focus update (sid: %d)", sid);
    AuraSurfaceWaylandData* data = aura_store_get(sState.surfaces, sid);
    if (!data) {
        // This is not a Wayland surface
        LOG_INFO3("SID %d does not resolve to any surface", sid);
        return 0;
    }

    struct wl_client* new_client = wl_resource_get_client(data->resource);
    struct wl_client* old_client = sState.keyboard_focused_client;

    struct wl_array array;
    wl_array_init(&array);

    // Check if new and old clients are different
    if (new_client == old_client) {
        return 0;
    }

    // Clear current client
    sState.keyboard_focused_client = NULL;

    // Send 'leave' and 'enter' event to all clients' keyboard objects
    Link* link = sState.keyboard_resources->first;
    while (link) {
        struct wl_resource* res = link->data;
        struct wl_client* res_client = wl_resource_get_client(res);

        if (res_client == old_client) {
            // TODO: wl_keyboard_send_leave
        }
        if (res_client == new_client) {
            wl_keyboard_send_enter(res, 0, data->resource, &array);
        }

        link = link->next;
    }

    // Update current client
    sState.keyboard_focused_client = new_client;

    pthread_mutex_unlock(&mutex);
    return 0;
}

//------------------------------------------------------------------------------

