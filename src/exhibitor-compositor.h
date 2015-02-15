// file: exhibitor-compositor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_COMPOSITOR_H__
#define __AURA_EXHIBITOR_COMPOSITOR_H__

#include "exhibitor-frame.h"
#include "utils-list.h"

typedef struct {
    AuraList* groups;
    AuraFrame* frame;
    AuraFrame* selection;
} AuraCompositor;

AuraCompositor* aura_compositor_new();

void aura_compositor_free(AuraCompositor* self);

AuraList* aura_compositor_get_visible_surfaces(AuraCompositor* self);

bool aura_compositor_manage_surface(AuraCompositor* self, AuraSurfaceId sid);

void aura_compositor_unmanage_surface(AuraCompositor* self, AuraSurfaceId sid);

void aura_compositor_pop_surface(AuraCompositor* self, AuraSurfaceId sid);

void aura_compositor_command_position(AuraCompositor* self,
                                      AuraArgmandType type,
                                      AuraArgmandType direction,
                                      int magnitude);

#endif // __AURA_EXHIBITOR_COMPOSITOR_H__

