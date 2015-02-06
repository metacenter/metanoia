// file: wayland-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-surface.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

AuraWaylandSurface* aura_wayland_surface_new()
{
    AuraWaylandSurface* self = malloc(sizeof(AuraWaylandSurface));
    if (!self) {
        return self;
    }

    memset(self, 0, sizeof(AuraWaylandSurface));
    return self;
}

//------------------------------------------------------------------------------

void aura_wayland_surface_free(AuraWaylandSurface* self)
{
    if (!self) {
        return;
    }

    if (self->resource) {
        wl_resource_destroy(self->resource);
    }
    if (self->buffer_resource) {
        wl_resource_destroy(self->buffer_resource);
    }
    if (self->frame_resource) {
        wl_resource_destroy(self->frame_resource);
    }
    if (self->shell_resource) {
        wl_resource_destroy(self->shell_resource);
    }
    if (self->xdg_shell_resource) {
        wl_resource_destroy(self->xdg_shell_resource);
    }

    memset(self, 0, sizeof(AuraWaylandSurface));
    free(self);
}

//------------------------------------------------------------------------------

