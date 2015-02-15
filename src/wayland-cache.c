// file: wayland-cache.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-cache.h"
#include "utils-log.h"

#include <pthread.h>

//------------------------------------------------------------------------------

pthread_mutex_t sCacheMutex = PTHREAD_MUTEX_INITIALIZER;

static struct {
    AuraStore* surfaces;
    AuraStore* regions;
    AuraList* surface_resource[AURA_NUM_SURFACE_RESOURCE_TYPES];
    AuraList* general_resource[AURA_NUM_GENERAL_RESOURCE_TYPES];
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
    sCache.regions = aura_store_new_for_id();
    if (!sCache.regions) {
        return AURA_RESULT_ERROR;
    }

    sCache.surfaces = aura_store_new_for_id();
    if (!sCache.surfaces) {
        return AURA_RESULT_ERROR;
    }

    for (int type = 0; type < AURA_NUM_SURFACE_RESOURCE_TYPES; ++type) {
        sCache.surface_resource[type] =
                              aura_list_new((AuraFreeFunc) wl_resource_destroy);
        if (!sCache.surface_resource[type]) {
            return AURA_RESULT_ERROR;
        }
    }

    for (int type = 0; type < AURA_NUM_GENERAL_RESOURCE_TYPES; ++type) {
        sCache.general_resource[type] =
                              aura_list_new((AuraFreeFunc) wl_resource_destroy);
        if (!sCache.general_resource[type]) {
            return AURA_RESULT_ERROR;
        }
    }

    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void aura_wayland_cache_finalize()
{
    for (int type = 0; type < AURA_NUM_GENERAL_RESOURCE_TYPES; ++type) {
        aura_list_free(sCache.general_resource[type]);
    }

    for (int type = 0; type < AURA_NUM_SURFACE_RESOURCE_TYPES; ++type) {
        aura_list_free(sCache.surface_resource[type]);
    }

    if (sCache.surfaces) {
        aura_store_free_with_items(sCache.surfaces,
                                  (AuraFreeFunc) aura_wayland_surface_free);
        sCache.surfaces = NULL;
    }

    if (sCache.regions) {
        aura_store_free_with_items(sCache.regions,
                                  (AuraFreeFunc) aura_wayland_region_free);
        sCache.regions = NULL;
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
        aura_wayland_surface_free(aura_store_delete(sCache.surfaces, sid));
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

AuraItemId aura_wayland_cache_create_region()
{
    AuraWaylandRegion* region = aura_wayland_region_new();
    AuraItemId rid = scInvalidItemId;
    if (region) {
        rid = aura_store_generate_new_id(sCache.regions);
        aura_store_add(sCache.regions, rid, region);
    }
    return rid;
}

//------------------------------------------------------------------------------

AuraWaylandRegion* aura_wayland_cache_find_region(AuraItemId rid)
{
    AuraWaylandRegion* result = NULL;
    if (rid != scInvalidItemId) {
        result = aura_store_find(sCache.regions, rid);
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

    aura_list_append(sCache.surface_resource[resource_type], resource);

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
        aura_list_append(sCache.general_resource[resource_type], resource);
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
        aura_list_remove(sCache.surface_resource[resource_type], resource,
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
        aura_list_remove(sCache.general_resource[resource_type], resource,
                     (AuraCompareFunc) aura_wayland_cache_compare_resources);
    } else {
        LOG_WARN1("Removing not existing resource type (%d)", resource_type);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

AuraList* aura_wayland_cache_get_resources
                                  (AuraWaylandGeneralResourceType resource_type)
{
    if (resource_type >= AURA_NUM_GENERAL_RESOURCE_TYPES) {
        return NULL;
    }
    return sCache.general_resource[resource_type];
}

//------------------------------------------------------------------------------

