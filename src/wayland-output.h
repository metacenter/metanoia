// file: wayland-output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_OUTPUT_H
#define NOIA_WAYLAND_OUTPUT_H

#include "utils-store.h"
#include "output.h"

#include <wayland-server.h>

typedef struct {
    NoiaItem base;
    struct wl_global* global_output;
    NoiaOutput* output;
} NoiaWaylandOutput;

NoiaWaylandOutput* noia_wayland_output_create(struct wl_global* global_output,
                                              NoiaOutput* output);

void noia_wayland_output_destroy(NoiaWaylandOutput* self);

#endif // NOIA_WAYLAND_OUTPUT_H

