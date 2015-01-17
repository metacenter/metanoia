// file: wayland-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-output.h"

#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraWaylandOutput* aura_wayland_output_new(struct wl_global* global_output)
{
    AuraWaylandOutput* self = malloc(sizeof(AuraWaylandOutput));
    if (!self) {
        LOG_ERROR("Memory allocation failure");
        return self;
    }

    self->base.str = NULL;
    self->global_output = global_output;
    return self;
}

//------------------------------------------------------------------------------

void aura_wayland_output_free(AuraWaylandOutput* self)
{
    if (!self) {
        return;
    }
    free(self);
}

//------------------------------------------------------------------------------

