// file: wayland-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-surface.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

AuraWaylandSurface* aura_wayland_surface_new()
{
    return calloc(sizeof(AuraWaylandSurface), 1);
}

//------------------------------------------------------------------------------

void aura_wayland_surface_free(AuraWaylandSurface* self)
{
    if (!self) {
        return;
    }

    if (self->resources[AURA_RESOURCE_FRAME]) {
        wl_resource_destroy(self->resources[AURA_RESOURCE_FRAME]);
    }

    memset(self, 0, sizeof(AuraWaylandSurface));
    free(self);
}

//------------------------------------------------------------------------------

struct wl_resource* aura_wayland_surface_get_resource
                                  (AuraWaylandSurface* self,
                                   AuraWaylandSurfaceResourceType resource_type)
{
    struct wl_resource* result = NULL;
    if (!self) {
        return result;
    }

    if (resource_type < AURA_NUM_SURFACE_RESOURCE_TYPES) {
        result = self->resources[resource_type];
    } else {
        LOG_WARN1("Reading not existing resource type (%d)", resource_type);
    }
    return result;
}

//------------------------------------------------------------------------------

void aura_wayland_surface_set_resource
                                  (AuraWaylandSurface* self,
                                   AuraWaylandSurfaceResourceType resource_type,
                                   struct wl_resource* resource)
{
    if (!self) {
        return;
    }

    if (resource_type < AURA_NUM_SURFACE_RESOURCE_TYPES) {
        self->resources[resource_type] = resource;
    } else {
        LOG_WARN1("Adding not existing resource type (%d)", resource_type);
    }
}

//------------------------------------------------------------------------------

