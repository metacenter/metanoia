// file: wayland-cache.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-cache.h"
#include "utils-log.h"

#include <pthread.h>

//------------------------------------------------------------------------------

pthread_mutex_t sCacheMutex = PTHREAD_MUTEX_INITIALIZER;

static struct {
    AuraStore* surfaces;
    Chain* surface_resource[AURA_NUM_SURFACE_RESOURCE_TYPES];
    Chain* general_resource[AURA_NUM_GENERAL_RESOURCE_TYPES];
} sCache;

//------------------------------------------------------------------------------

int aura_wayland_cache_compare_resources(struct wl_resource* rc1,
                                         struct wl_resource* rc2)
{
    if (rc1 < rc2) {
        return -1;
    } else if (rc1 > rc2) {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------

AuraResult aura_wayland_cache_initialize()
{
    sCache.surfaces = aura_store_new_for_id();
    if (!sCache.surfaces) {
        return AURA_RESULT_ERROR;
    }

    for (int i = 0; i < AURA_NUM_SURFACE_RESOURCE_TYPES; ++i) {
        sCache.surface_resource[i] = chain_new(NULL);
        if (!sCache.surface_resource[i]) {
            return AURA_RESULT_ERROR;
        }
    }

    for (int i = 0; i < AURA_NUM_GENERAL_RESOURCE_TYPES; ++i) {
        sCache.general_resource[i] =
                                  chain_new((AuraFreeFunc) wl_resource_destroy);
        if (!sCache.general_resource[i]) {
            return AURA_RESULT_ERROR;
        }
    }

    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void aura_wayland_cache_finalize()
{
    for (int i = 0; i < AURA_NUM_GENERAL_RESOURCE_TYPES; ++i) {
        chain_free(sCache.general_resource[i]);
    }

    for (int i = 0; i < AURA_NUM_SURFACE_RESOURCE_TYPES; ++i) {
        chain_free(sCache.surface_resource[i]);
    }

    if (sCache.surfaces) {
        aura_store_free_with_items(sCache.surfaces,
                                  (AuraFreeFunc) aura_wayland_surface_free);
        sCache.surfaces = NULL;
    }
}

//------------------------------------------------------------------------------

void aura_wayland_cache_add_surface(AuraSurfaceId sid,
                                    AuraWaylandSurface* surface)
{
    if (sid != scInvalidSurfaceId) {
        aura_store_add(sCache.surfaces, sid, surface);
    }
}

//------------------------------------------------------------------------------

void aura_wayland_cache_remove_surface(AuraSurfaceId sid)
{
    if (sid != scInvalidSurfaceId) {
        aura_store_delete(sCache.surfaces, sid);
    }
}

//------------------------------------------------------------------------------

AuraWaylandSurface* aura_wayland_cache_find_surface(AuraSurfaceId sid)
{
    AuraWaylandSurface* result = NULL;
    if (sid != scInvalidSurfaceId) {
        result = aura_store_find(sCache.surfaces, sid);
    }
    return result;
}

//------------------------------------------------------------------------------

void aura_wayland_cache_add_surface_resource
                                  (AuraSurfaceId sid,
                                   AuraWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    chain_append(sCache.surface_resource[resource_type], resource);

    AuraWaylandSurface* surface = aura_store_find(sCache.surfaces, sid);
    if (surface) {
        aura_wayland_surface_set_resource(surface, resource_type, resource);
    } else {
        LOG_INFO3("SID %d does not resolve to any surface", sid);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_cache_add_general_resource
                                   (AuraWaylandGeneralResourceType resource_type,
                                    struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    if (resource_type < AURA_NUM_GENERAL_RESOURCE_TYPES) {
        chain_append(sCache.general_resource[resource_type], resource);
    } else {
        LOG_WARN1("Adding not existing resource type (%d)", resource_type);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_cache_remove_surface_resource
                                  (AuraWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    if (resource_type < AURA_NUM_SURFACE_RESOURCE_TYPES) {
        chain_remove(sCache.surface_resource[resource_type], resource,
                     (AuraCompareFunc) aura_wayland_cache_compare_resources);
    } else {
        LOG_WARN1("Removing not existing resource type (%d)", resource_type);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

void aura_wayland_cache_remove_general_resource
                                  (AuraWaylandGeneralResourceType resource_type,
                                   struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    if (resource_type < AURA_NUM_GENERAL_RESOURCE_TYPES) {
        chain_remove(sCache.general_resource[resource_type], resource,
                     (AuraCompareFunc) aura_wayland_cache_compare_resources);
    } else {
        LOG_WARN1("Removing not existing resource type (%d)", resource_type);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

Chain* aura_wayland_cache_get_resources
                                  (AuraWaylandGeneralResourceType resource_type)
{
    if (resource_type >= AURA_NUM_GENERAL_RESOURCE_TYPES) {
        return NULL;
    }
    return sCache.general_resource[resource_type];
}

//------------------------------------------------------------------------------

