// file: wayland-output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_OUTPUT_H__
#define __AURA_WAYLAND_OUTPUT_H__

#include "utils-store.h"

#include <wayland-server.h>

typedef struct {
    AuraItem base;
    struct wl_global* global_output;
} AuraWaylandOutput;

AuraWaylandOutput* aura_wayland_output_new(struct wl_global* global_output);
void aura_wayland_output_free(AuraWaylandOutput* self);

#endif // __AURA_WAYLAND_OUTPUT_H__

