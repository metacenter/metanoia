// file: wayland-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-output.h"

#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaWaylandOutput* noia_wayland_output_create(struct wl_global* global_output,
                                              NoiaOutput* output)
{
    NoiaWaylandOutput* self = malloc(sizeof(NoiaWaylandOutput));
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

void noia_wayland_output_destroy(NoiaWaylandOutput* self)
{
    if (!self) {
        return;
    }

    noia_object_unref((NoiaObject*) self->output);
    if (self->base.str) {
        free(self->base.str);
    }
    memset(self, 0, sizeof(NoiaWaylandOutput));
    free(self);
}

//------------------------------------------------------------------------------

