// file: exhibitor-compositor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_COMPOSITOR_H__
#define __NOIA_EXHIBITOR_COMPOSITOR_H__

#include "exhibitor-frame.h"

typedef struct {
    NoiaFrame* root;
    NoiaFrame* selection;
} NoiaCompositor;

NoiaCompositor* noia_compositor_new();

void noia_compositor_free(NoiaCompositor* self);

NoiaFrame* noia_compositor_create_new_workspace(NoiaCompositor* self,
                                                NoiaSize size);

bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

void noia_compositor_pop_surface(NoiaCompositor* self, NoiaSurfaceId sid);

void noia_compositor_command_anchorize(NoiaCompositor* self);

void noia_compositor_command_position(NoiaCompositor* self,
                                      NoiaArgmandType type,
                                      NoiaArgmandType direction,
                                      int magnitude);

#endif // __NOIA_EXHIBITOR_COMPOSITOR_H__

