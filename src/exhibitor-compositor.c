// file: exhibitor-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-compositor.h"

#include "surface-manager.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

AuraCompositor* aura_compositor_new()
{
    AuraCompositor* self = malloc(sizeof(AuraCompositor));
    if (!self) {
        LOG_ERROR("Could not create new compositor!");
        return self;
    }

    self->groups = aura_list_new(NULL);
    self->frame = aura_frame_new();
    aura_frame_set_type(self->frame, AURA_FRAME_TYPE_STACKED);
    return self;
}

//------------------------------------------------------------------------------

void aura_compositor_free(AuraCompositor* self)
{
    if (!self) {
        return;
    }

    aura_frame_free(self->frame);
    aura_list_free(self->groups);

    memset(self, 0, sizeof(AuraCompositor));
    free(self);
}

//------------------------------------------------------------------------------

AuraList* aura_compositor_get_visible_surfaces(AuraCompositor* self)
{
    /// @todo Reimplement aura_compositor_get_visible_surfaces.
    /// And don't use aura_frame_get_params - it should be `static inline`.
    AuraList* surfaces = aura_list_new(NULL);
    FOR_EACH_TWIG(self->frame, twig) {
        AuraFrameParams* params = aura_frame_get_params(twig);
        aura_list_append(surfaces, (void*) params->sid);
    }
    return surfaces;
}

//------------------------------------------------------------------------------

void aura_compositor_manage_surface(AuraCompositor* self, AuraSurfaceId sid)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface || !surface->is_toplevel) {
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

/// Pop up a surface if covered with others.
///
/// Here `aura_frame_find_with_sid` is used. Pointer to Frame could also be
/// stored in SurfaceData for faster access, but this would require carrying
/// about this pointer this other operation (e.g. jumps) which also would
/// require additional computations. Pop is done very rarely and we do not have
/// overwhelming number of surfaces, so searching is justified here.
///
/// @see aura_frame_find_with_sid
void aura_compositor_pop_surface(AURA_UNUSED AuraCompositor* self,
                                 AURA_UNUSED AuraSurfaceId sid)
{
    AuraFrame* frame = aura_frame_find_with_sid(self->frame, sid);
    aura_frame_pop_recursively(self->frame, frame);
    self->selection = frame;
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

