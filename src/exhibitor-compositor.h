// file: exhibitor-compositor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_COMPOSITOR_H__
#define __AURA_EXHIBITOR_COMPOSITOR_H__

#include "exhibitor-frame.h"
#include "global-types.h"

typedef struct {
    Chain* groups;
    AuraFrame* frame;
} AuraCompositor;

AuraCompositor* aura_compositor_new();

Chain* aura_compositor_get_visible_surfaces(AuraCompositor* self);

void aura_compositor_manage_surface(AuraCompositor* self, SurfaceId sid);

void aura_compositor_pop_surface(AuraCompositor* self, SurfaceId sid);

void aura_compositor_command_position(AuraCompositor* self,
                                      AuraArgmandType type,
                                      AuraArgmandType direction,
                                      int position);

#endif // __AURA_EXHIBITOR_COMPOSITOR_H__

