// file: wayland-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-surface.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

int noia_wayland_surfsace_compare_resources(struct wl_resource* rc1,
                                            struct wl_resource* rc2)
{
    return rc1 != rc2;
}

//------------------------------------------------------------------------------

NoiaWaylandSurface* noia_wayland_surface_new(void)
{
    NoiaWaylandSurface* self = calloc(1, sizeof(NoiaWaylandSurface));
    self->frame_resources = noia_list_new(NULL);
    return self;
}

//------------------------------------------------------------------------------

void noia_wayland_surface_free(NoiaWaylandSurface* self)
{
    NOIA_ENSURE(self, return);

    int len = noia_list_len(self->frame_resources);
    if (len > 2) {
        LOG_WARN1("Wayland: %d surface frame resources not released!", len);
    }
    noia_wayland_surface_remove_frame_resources(self);
    noia_list_free(self->frame_resources);

    memset(self, 0, sizeof(NoiaWaylandSurface));
    free(self);
}

//------------------------------------------------------------------------------

struct wl_resource* noia_wayland_surface_get_resource
                                  (NoiaWaylandSurface* self,
                                   NoiaWaylandSurfaceResourceType resource_type)
{
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(resource_type < NOIA_NUM_SURFACE_RESOURCE_TYPES, return NULL);

    return self->resources[resource_type];
}

//------------------------------------------------------------------------------

const NoiaList* noia_wayland_surface_get_frame_resources
                                                      (NoiaWaylandSurface* self)
{
    NOIA_ENSURE(self, return NULL);
    return self->frame_resources;
}

//------------------------------------------------------------------------------

void noia_wayland_surface_add_resource
                                  (NoiaWaylandSurface* self,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(resource_type < NOIA_NUM_SURFACE_RESOURCE_TYPES, return);

    if (resource_type == NOIA_RESOURCE_FRAME) {
        noia_list_append(self->frame_resources, resource);
    } else if (self->resources[resource_type]) {
        LOG_WAYL3("Wayland: surface resource of type '%d' "
                  "already here!", resource_type);
    }

    self->resources[resource_type] = resource;
}

//------------------------------------------------------------------------------

void noia_wayland_surface_remove_resource
                                  (NoiaWaylandSurface* self,
                                   NoiaWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(resource_type < NOIA_NUM_SURFACE_RESOURCE_TYPES, return);

    self->resources[resource_type] = NULL;

    if (resource_type == NOIA_RESOURCE_FRAME) {
        noia_list_remove(self->frame_resources, resource,
                     (NoiaCompareFunc) noia_wayland_surfsace_compare_resources);
        self->resources[resource_type] = noia_list_first(self->frame_resources);
    }
}

//------------------------------------------------------------------------------

void noia_wayland_surface_remove_frame_resources(NoiaWaylandSurface* self)
{
    NOIA_ENSURE(self, return);
    while (noia_list_len(self->frame_resources) > 0) {
        wl_resource_destroy(noia_list_first(self->frame_resources));
    }
}

//------------------------------------------------------------------------------

