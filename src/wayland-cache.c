// file: wayland-cache.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-cache.h"
#include "utils-log.h"

#include <pthread.h>

//------------------------------------------------------------------------------

pthread_mutex_t sCacheMutex = PTHREAD_MUTEX_INITIALIZER;

static struct {
    NoiaStore* surfaces;
    NoiaStore* regions;
    NoiaList* surface_resource[NOIA_NUM_SURFACE_RESOURCE_TYPES];
    NoiaList* general_resource[NOIA_NUM_GENERAL_RESOURCE_TYPES];
} sCache;

//------------------------------------------------------------------------------

int noia_wayland_cache_compare_resources(struct wl_resource* rc1,
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

NoiaResult noia_wayland_cache_initialize(void)
{
    sCache.regions = noia_store_new_for_id();
    if (!sCache.regions) {
        return NOIA_RESULT_ERROR;
    }

    sCache.surfaces = noia_store_new_for_id();
    if (!sCache.surfaces) {
        return NOIA_RESULT_ERROR;
    }

    for (int type = 0; type < NOIA_NUM_SURFACE_RESOURCE_TYPES; ++type) {
        sCache.surface_resource[type] = noia_list_new(NULL);
        if (!sCache.surface_resource[type]) {
            return NOIA_RESULT_ERROR;
        }
    }

    for (int type = 0; type < NOIA_NUM_GENERAL_RESOURCE_TYPES; ++type) {
        sCache.general_resource[type] = noia_list_new(NULL);
        if (!sCache.general_resource[type]) {
            return NOIA_RESULT_ERROR;
        }
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_wayland_cache_finalize(void)
{
    for (int type = 0; type < NOIA_NUM_GENERAL_RESOURCE_TYPES; ++type) {
        noia_list_free(sCache.general_resource[type]);
    }

    for (int type = 0; type < NOIA_NUM_SURFACE_RESOURCE_TYPES; ++type) {
        noia_list_free(sCache.surface_resource[type]);
    }

    if (sCache.surfaces) {
        noia_store_free_with_items(sCache.surfaces,
                                  (NoiaFreeFunc) noia_wayland_surface_free);
        sCache.surfaces = NULL;
    }

    if (sCache.regions) {
        noia_store_free_with_items(sCache.regions,
                                  (NoiaFreeFunc) noia_wayland_region_free);
        sCache.regions = NULL;
    }
}

//------------------------------------------------------------------------------

void noia_wayland_cache_add_surface(NoiaSurfaceId sid,
                                    NoiaWaylandSurface* surface)
{
    if (sid != scInvalidSurfaceId) {
        noia_store_add(sCache.surfaces, sid, surface);
    }
}

//------------------------------------------------------------------------------

void noia_wayland_cache_remove_surface(NoiaSurfaceId sid)
{
    if (sid != scInvalidSurfaceId) {
        noia_wayland_surface_free(noia_store_delete(sCache.surfaces, sid));
    }
}

//------------------------------------------------------------------------------

NoiaWaylandSurface* noia_wayland_cache_find_surface(NoiaSurfaceId sid)
{
    NoiaWaylandSurface* result = NULL;
    if (sid != scInvalidSurfaceId) {
        result = noia_store_find(sCache.surfaces, sid);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaItemId noia_wayland_cache_create_region(void)
{
    NoiaWaylandRegion* region = noia_wayland_region_new();
    NoiaItemId rid = scInvalidItemId;
    if (region) {
        rid = noia_store_generate_new_id(sCache.regions);
        noia_store_add(sCache.regions, rid, region);
    }
    return rid;
}

//------------------------------------------------------------------------------

NoiaWaylandRegion* noia_wayland_cache_find_region(NoiaItemId rid)
{
    NoiaWaylandRegion* result = NULL;
    if (rid != scInvalidItemId) {
        result = noia_store_find(sCache.regions, rid);
    }
    return result;
}

//------------------------------------------------------------------------------

void noia_wayland_cache_add_surface_resource
                                  (NoiaSurfaceId sid,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    noia_list_append(sCache.surface_resource[resource_type], resource);

    NoiaWaylandSurface* surface = noia_store_find(sCache.surfaces, sid);
    if (surface) {
        noia_wayland_surface_set_resource(surface, resource_type, resource);
    } else {
        LOG_INFO3("SID %d does not resolve to any surface", sid);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_cache_add_general_resource
                                   (NoiaWaylandGeneralResourceType resource_type,
                                    struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    if (resource_type < NOIA_NUM_GENERAL_RESOURCE_TYPES) {
        noia_list_append(sCache.general_resource[resource_type], resource);
    } else {
        LOG_WARN1("Adding not existing resource type (%d)", resource_type);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_cache_remove_surface_resource
                                  (NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    if (resource_type < NOIA_NUM_SURFACE_RESOURCE_TYPES) {
        noia_list_remove(sCache.surface_resource[resource_type], resource,
                     (NoiaCompareFunc) noia_wayland_cache_compare_resources);
    } else {
        LOG_WARN1("Removing not existing resource type (%d)", resource_type);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

void noia_wayland_cache_remove_general_resource
                                  (NoiaWaylandGeneralResourceType resource_type,
                                   struct wl_resource* resource)
{
    pthread_mutex_lock(&sCacheMutex);

    if (resource_type < NOIA_NUM_GENERAL_RESOURCE_TYPES) {
        noia_list_remove(sCache.general_resource[resource_type], resource,
                     (NoiaCompareFunc) noia_wayland_cache_compare_resources);
    } else {
        LOG_WARN1("Removing not existing resource type (%d)", resource_type);
    }

    pthread_mutex_unlock(&sCacheMutex);
}

//------------------------------------------------------------------------------

NoiaList* noia_wayland_cache_get_resources
                                  (NoiaWaylandGeneralResourceType resource_type)
{
    if (resource_type >= NOIA_NUM_GENERAL_RESOURCE_TYPES) {
        return NULL;
    }
    return sCache.general_resource[resource_type];
}

//------------------------------------------------------------------------------

