// file: wayland-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-output.h"

#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

AuraWaylandOutput* aura_wayland_output_create(struct wl_global* global_output,
                                              AuraOutput* output)
{
    AuraWaylandOutput* self = malloc(sizeof(AuraWaylandOutput));
    if (!self) {
        LOG_ERROR("Memory allocation failure");
        return self;
    }

    self->base.str = NULL;
    self->global_output = global_output;
    self->output = output;
    return self;
}

//------------------------------------------------------------------------------

void aura_wayland_output_destroy(AuraWaylandOutput* self)
{
    if (!self) {
        return;
    }

    aura_object_unref((AuraObject*) self->output);
    if (self->base.str) {
        free(self->base.str);
    }
    memset(self, 0, sizeof(AuraWaylandOutput));
    free(self);
}

//------------------------------------------------------------------------------

