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
    aura_frame_set_type(self->frame, AURA_FRAME_TYPE_WORKSPACE);
    return self;
}

//------------------------------------------------------------------------------

Chain* aura_compositor_get_visible_surfaces(AuraCompositor* self)
{
    /// @todo Reimplement aura_compositor_get_visible_surfaces
    Chain* surfaces = chain_new(NULL);
    FOR_EACH_TWIG(self->frame, twig) {
        AuraFrameParams* params = aura_frame_get_params(twig);
        chain_append(surfaces, (void*) params->sid);
    }
    return surfaces;
}

//------------------------------------------------------------------------------

void aura_compositor_manage_surface(AuraCompositor* self, SurfaceId sid)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface) {
        return;
    }

    /// @todo Does surface need compositor?
    surface->group.compositor = self;

    /// @todo Frame type should be configurable.
    AuraFrame* frame = aura_frame_new();
    aura_frame_set_type(frame, AURA_FRAME_TYPE_FLOATING);
    aura_frame_set_surface(frame, sid);

    aura_branch_append(self->frame, frame);
    self->selection = frame;
}

//------------------------------------------------------------------------------

void aura_compositor_pop_surface(AURA_UNUSED AuraCompositor* self,
                                 AURA_UNUSED SurfaceId sid)
{
    /// @todo Reimplement aura_compositor_pop_surface
    //chain_remove(self->frame->twigs, (void*) sid,
    //             (AuraCompareFunc) aura_surface_compare);
    //chain_append(self->frame->twigs, (void*) sid);
    //self->selection = (AuraFrame*) self->frame->twigs->first;
}

//------------------------------------------------------------------------------

void aura_compositor_command_position(AuraCompositor* self,
                                      AuraArgmandType type,
                                      AuraArgmandType direction,
                                      int magnitude)
{
    if (!self->selection) {
        return;
    }

    /// @todo Magnitude scale should be configurable
    int scale = 10;
    if (type == AURA_ARGMAND_RESIZE) {
        aura_frame_resize(self->selection, direction, scale*magnitude);
    } else if (type == AURA_ARGMAND_MOVE) {
        aura_frame_move(self->selection, direction, scale*magnitude);
    } else if (type == AURA_ARGMAND_JUMP) {
        aura_frame_jump(self->selection, direction, magnitude);
    } else if (type == AURA_ARGMAND_FOCUS) {
        /// @todo Handle AURA_ARGMAND_FOCUS
    }
}

//------------------------------------------------------------------------------

