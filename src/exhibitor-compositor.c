// file: exhibitor-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-compositor.h"
#include "exhibitor.h"

#include "surface-manager.h"
#include "utils-log.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaCompositor* noia_compositor_new()
{
    NoiaCompositor* self = malloc(sizeof(NoiaCompositor));
    assert(self);

    NoiaPosition pos = {0, 0};
    NoiaSize size = {-1, -1};
    self->root = noia_frame_new();
    noia_frame_configure(self->root, NOIA_FRAME_TYPE_STACKED,
                         scInvalidSurfaceId, pos, size);
    return self;
}

//------------------------------------------------------------------------------

void noia_compositor_free(NoiaCompositor* self)
{
    assert(self);

    noia_frame_free(self->root);

    memset(self, 0, sizeof(NoiaCompositor));
    free(self);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_compositor_create_new_workspace(NoiaCompositor* self,
                                                NoiaSize size)
{
    assert(self);

    NoiaFrame* workspace = noia_frame_new();
    noia_frame_configure_as_workspace(workspace, size);
    noia_frame_append(self->root, workspace);

    /// @todo This should be configurable
    self->selection = workspace;
    return workspace;
}

//------------------------------------------------------------------------------

bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    assert(self);

    NoiaSurfaceData* surface = noia_surface_get(sid);
    if (!surface || !surface->is_toplevel) {
        return false;
    }

    /// @todo Frame type, size and position should be configurable.
    NoiaFrame* frame = noia_frame_new();
    noia_frame_configure(frame, NOIA_FRAME_TYPE_FLOATING, sid,
                         (NoiaPosition) {0,0}, surface->requested_size);

    noia_frame_append(self->selection, frame);
    /// @todo This should be configurable
    self->selection = frame;

    noia_frame_log(self->root);

    return true;
}

//------------------------------------------------------------------------------

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    assert(self);
    NoiaFrame* frame = noia_frame_find_with_sid(self->root, sid);
    noia_frame_remove_self(frame);
    noia_frame_free(frame);
}

//------------------------------------------------------------------------------

/// Here `noia_frame_find_with_sid` is used. Pointer to Frame could also be
/// stored in SurfaceData for faster access, but this would require carrying
/// about this pointer this other operation (e.g. jumps) which also would
/// require additional computations. Pop is done very rarely and we do not have
/// overwhelming number of surfaces, so searching is justified here.
void noia_compositor_pop_surface(NoiaCompositor* self,
                                 NoiaSurfaceId sid)
{
    assert(self);

    // Pop in frame hierarchy.
    NoiaFrame* frame = noia_frame_find_with_sid(self->root, sid);
    noia_frame_pop_recursively(self->root, frame);

    // Pop in history.
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
    noia_list_remove(exhibitor->surface_history,
                    (void*) sid, (NoiaCompareFunc) noia_surface_compare);
    noia_list_prepend(exhibitor->surface_history, (void*) sid);

    // Update selection.
    self->selection = frame;
}

//------------------------------------------------------------------------------

void noia_compositor_focus(NoiaCompositor* self,
                           NoiaArgmand argmand,
                           int position)
{
    assert(noia_argmand_is_directed(argmand));

    if (argmand == NOIA_ARGMAND_BACK || argmand == NOIA_ARGMAND_FORWARD) {
        NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
        if (argmand == NOIA_ARGMAND_BACK) {
            position = -1 * position;
            argmand = NOIA_ARGMAND_FORWARD;
        }

        NoiaSurfaceId sid = (NoiaSurfaceId) noia_list_get_nth
                                         (exhibitor->surface_history, position);
        if (sid != scInvalidSurfaceId) {
            noia_compositor_pop_surface(self, sid);
        }
    } else {
        self->selection = noia_frame_find_pointed(self->selection,
                                                  argmand, position);
    }
}

//------------------------------------------------------------------------------

void noia_compositor_anchorize(NoiaCompositor* self, NoiaFrame* frame)
{
    assert(self);
    assert(frame);

    NOIA_TRY {
        if (!noia_frame_has_type(self->selection, NOIA_FRAME_TYPE_FLOATING)) {
            break;
        }

        NoiaFrame* workspace = noia_frame_get_top(self->selection);
        if (!workspace) {
            break;
        }

        noia_frame_resettle(frame, workspace);
    }
    noia_frame_log(self->root);
}

//------------------------------------------------------------------------------

void noia_compositor_execute(NoiaCompositor* self, NoiaAction* a)
{
    assert(a);
    assert(self);
    assert(noia_argmand_is_actionable(a->action));
    if (!self->selection) {
        return;
    }

    switch (a->action) {
    case NOIA_ARGMAND_RESIZE:
        noia_frame_resize(self->selection, a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_MOVE:
        noia_frame_move(self->selection, a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_JUMP:
        noia_frame_jump(self->selection, a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_FOCUS:
        noia_compositor_focus(self, a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_CONF:
        break;
    case NOIA_ARGMAND_ANCHOR:
        noia_compositor_anchorize(self, self->selection);
        break;
    default: break;
    }
}

//------------------------------------------------------------------------------

