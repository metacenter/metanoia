// file: exhibitor-compositor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_COMPOSITOR_H__
#define __NOIA_EXHIBITOR_COMPOSITOR_H__

#include "exhibitor-frame.h"
#include "utils-list.h"

typedef struct {
    NoiaList* groups;
    NoiaFrame* frame;
    NoiaFrame* selection;
} NoiaCompositor;

NoiaCompositor* noia_compositor_new();

void noia_compositor_free(NoiaCompositor* self);

NoiaList* noia_compositor_get_visible_surfaces(NoiaCompositor* self);

bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

void noia_compositor_pop_surface(NoiaCompositor* self, NoiaSurfaceId sid);

void noia_compositor_command_position(NoiaCompositor* self,
                                      NoiaArgmandType type,
                                      NoiaArgmandType direction,
                                      int magnitude);

#endif // __NOIA_EXHIBITOR_COMPOSITOR_H__

