// file: wayland-region.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-region.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaWaylandRegion* noia_wayland_region_new()
{
    return calloc(sizeof(NoiaWaylandRegion), 1);
}

//------------------------------------------------------------------------------

void noia_wayland_region_free(NoiaWaylandRegion* self)
{
    if (!self) {
        return;
    }

    memset(self, 0, sizeof(NoiaWaylandRegion));
    free(self);
}

//------------------------------------------------------------------------------

bool noia_wayland_region_is_valid(NoiaWaylandRegion* self)
{
    if (!self) {
        return false;
    }
    return self->pos.x > 0 && self->pos.y > 0
        && self->size.width > 0 && self->size.height > 0;
}

//------------------------------------------------------------------------------

void noia_wayland_region_inflate(NoiaWaylandRegion* self,
                                 int x, int y,
                                 int width, int height)
{
    int diff = 0;

    if (noia_wayland_region_is_valid(self)) {
        NoiaWaylandRegion old = *self;

        diff = old.pos.x - x;
        if (diff > 0) {
            self->size.width += diff;
            self->pos.x = x;
        }

        diff = old.pos.y - y;
        if (diff > 0) {
            self->size.width += diff;
            self->pos.y = y;
        }

        diff = old.pos.x + old.size.width - x - width;
        if (diff < 0) {
            self->size.width -= diff;
        }

        diff = old.pos.y + old.size.height - y - height;
        if (diff < 0) {
            self->size.height -= diff;
        }
    } else {
        self->pos.x = x;
        self->pos.y = y;
        self->size.width = width;
        self->size.height = height;
    }
}

//------------------------------------------------------------------------------

