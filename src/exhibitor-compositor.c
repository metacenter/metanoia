// file: exhibitor-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-compositor.h"

#include "surface-manager.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaCompositor* noia_compositor_new()
{
    NoiaCompositor* self = malloc(sizeof(NoiaCompositor));
    if (!self) {
        LOG_ERROR("Could not create new compositor!");
        return self;
    }

    self->groups = noia_list_new(NULL);
    self->frame = noia_frame_new();
    noia_frame_set_type(self->frame, NOIA_FRAME_TYPE_STACKED);
    return self;
}

//------------------------------------------------------------------------------

void noia_compositor_free(NoiaCompositor* self)
{
    if (!self) {
        return;
    }

    noia_frame_free(self->frame);
    noia_list_free(self->groups);

    memset(self, 0, sizeof(NoiaCompositor));
    free(self);
}

//------------------------------------------------------------------------------

NoiaList* noia_compositor_get_visible_surfaces(NoiaCompositor* self)
{
    /// @todo Reimplement noia_compositor_get_visible_surfaces.
    /// And don't use noia_frame_get_params - it should be `static inline`.
    NoiaList* surfaces = noia_list_new(NULL);
    FOR_EACH_TWIG(self->frame, twig) {
        NoiaFrameParams* params = noia_frame_get_params(twig);
        noia_list_append(surfaces, (void*) params->sid);
    }
    return surfaces;
}

//------------------------------------------------------------------------------

bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NoiaSurfaceData* surface = noia_surface_get(sid);
    if (!surface || !surface->is_toplevel) {
        return false;
    }

    /// @todo Does surface need compositor?
    surface->group.compositor = self;

    /// @todo Frame type should be configurable.
    NoiaFrame* frame = noia_frame_new();
    noia_frame_set_type(frame, NOIA_FRAME_TYPE_FLOATING);
    noia_frame_set_surface(frame, sid);

    noia_branch_append(self->frame, frame);
    self->selection = frame;

    return true;
}

//------------------------------------------------------------------------------

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NoiaFrame* frame = noia_frame_find_with_sid(self->frame, sid);
    noia_frame_remove_self(frame);
    noia_frame_free(frame);
}

//------------------------------------------------------------------------------

/// Pop up a surface if covered with others.
///
/// Here `noia_frame_find_with_sid` is used. Pointer to Frame could also be
/// stored in SurfaceData for faster access, but this would require carrying
/// about this pointer this other operation (e.g. jumps) which also would
/// require additional computations. Pop is done very rarely and we do not have
/// overwhelming number of surfaces, so searching is justified here.
///
/// @see noia_frame_find_with_sid
void noia_compositor_pop_surface(NOIA_UNUSED NoiaCompositor* self,
                                 NOIA_UNUSED NoiaSurfaceId sid)
{
    NoiaFrame* frame = noia_frame_find_with_sid(self->frame, sid);
    noia_frame_pop_recursively(self->frame, frame);
    self->selection = frame;
}

//------------------------------------------------------------------------------

void noia_compositor_command_position(NoiaCompositor* self,
                                      NoiaArgmandType type,
                                      NoiaArgmandType direction,
                                      int magnitude)
{
    if (!self->selection) {
        return;
    }

    /// @todo Magnitude scale should be configurable
    int scale = 10;
    if (type == NOIA_ARGMAND_RESIZE) {
        noia_frame_resize(self->selection, direction, scale*magnitude);
    } else if (type == NOIA_ARGMAND_MOVE) {
        noia_frame_move(self->selection, direction, scale*magnitude);
    } else if (type == NOIA_ARGMAND_JUMP) {
        noia_frame_jump(self->selection, direction, magnitude);
    } else if (type == NOIA_ARGMAND_FOCUS) {
        /// @todo Handle NOIA_ARGMAND_FOCUS
    }
}

//------------------------------------------------------------------------------

