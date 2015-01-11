// file: exhibitor-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-compositor.h"

#include "surface-manager.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraCompositor* aura_compositor_new()
{
    AuraCompositor* self = malloc(sizeof(AuraCompositor));
    if (!self) {
        LOG_ERROR("Could not create new compositor!");
        return self;
    }

    self->groups = chain_new(0);
    self->frame = aura_frame_new();
    return self;
}

//------------------------------------------------------------------------------

Chain* aura_compositor_get_visible_surfaces(AuraCompositor* self)
{
    return self->frame->subbranches;
}

//------------------------------------------------------------------------------

void aura_compositor_manage_surface(AuraCompositor* self, SurfaceId sid)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface) {
        return;
    }

    surface->group.compositor = self;
    chain_append(self->frame->subbranches, (void*) sid);
}

//------------------------------------------------------------------------------

void aura_compositor_pop_surface(AuraCompositor* self, SurfaceId sid)
{
    chain_remove(self->frame->subbranches, (void*) sid,
                 (AuraCompareFunc) aura_surface_compare);
    chain_append(self->frame->subbranches, (void*) sid);
}

//------------------------------------------------------------------------------

void aura_compositor_command_position(AuraCompositor* self,
                                      AuraArgmandType type,
                                      AuraArgmandType direction,
                                      int position)
{
    // TODO
}

//------------------------------------------------------------------------------

