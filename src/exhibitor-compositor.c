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

    NoiaPosition pos = {0, 0};
    NoiaSize size = {-1, -1};
    self->root = noia_frame_create(pos, size);
    noia_frame_set_type(self->root, NOIA_FRAME_TYPE_STACKED);
    return self;
}

//------------------------------------------------------------------------------

void noia_compositor_free(NoiaCompositor* self)
{
    if (!self) {
        return;
    }

    noia_frame_free(self->root);

    memset(self, 0, sizeof(NoiaCompositor));
    free(self);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_compositor_create_new_workspace(NoiaCompositor* self,
                                                unsigned width,
                                                unsigned height)
{
    if (!self) {
        return NULL;
    }

    NoiaPosition pos = {0, 0};
    NoiaSize size = {width, height};
    NoiaFrame* workspace = noia_frame_create(pos, size);
    noia_frame_init_as_workspace(workspace);
    noia_frame_append(self->root, workspace);

    /// @todo This should be configurable
    self->selection = workspace;
    return workspace;
}

//------------------------------------------------------------------------------

bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NoiaSurfaceData* surface = noia_surface_get(sid);
    if (!surface || !surface->is_toplevel) {
        return false;
    }

    /// @todo Frame type, size and position should be configurable.
    NoiaPosition pos = {0, 0};
    NoiaFrame* frame = noia_frame_create(pos, surface->requested_size);
    noia_frame_set_type(frame, NOIA_FRAME_TYPE_FLOATING);
    noia_frame_set_surface(frame, sid);

    noia_frame_append(self->selection, frame);
    /// @todo This should be configurable
    self->selection = frame;

    noia_frame_log(self->root);

    return true;
}

//------------------------------------------------------------------------------

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NoiaFrame* frame = noia_frame_find_with_sid(self->root, sid);
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
void noia_compositor_pop_surface(NoiaCompositor* self,
                                 NoiaSurfaceId sid)
{
    NoiaFrame* frame = noia_frame_find_with_sid(self->root, sid);
    noia_frame_pop_recursively(self->root, frame);
    self->selection = frame;
}

//------------------------------------------------------------------------------

/// Anchorizes the surface.
void noia_compositor_command_anchorize(NoiaCompositor* self)
{
    NOIA_TRY {
        if (!self->selection) {
            break;
        }

        if (!noia_frame_has_type(self->selection, NOIA_FRAME_TYPE_FLOATING)) {
            break;
        }

        NoiaFrame* workspace = self->selection->trunk;
        NoiaFrame* frame = noia_frame_create_child(workspace,
                                                   NOIA_FRAME_TYPE_LEAF);
        if (!frame) {
            break;
        }

        noia_frame_resettle(self->selection, frame);
    }
    noia_frame_log(self->root);
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

