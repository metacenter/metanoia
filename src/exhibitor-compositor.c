// file: exhibitor-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-compositor.h"
#include "exhibitor.h"

#include "surface-manager.h"
#include "event-signals.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaCompositor* noia_compositor_new(void)
{
    NoiaCompositor* self = malloc(sizeof(NoiaCompositor));
    NOIA_ENSURE(self, abort());

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
    NOIA_ENSURE(self, return);

    noia_frame_free(self->root);

    memset(self, 0, sizeof(NoiaCompositor));
    free(self);
}

//------------------------------------------------------------------------------

void noia_compositor_set_selection(NoiaCompositor* self, NoiaFrame* frame)
{
    NOIA_ENSURE(frame, return);

    self->selection = frame;

    if (noia_frame_has_type(frame, NOIA_FRAME_TYPE_LEAF)) {
        NoiaSurfaceId sid = noia_frame_get_sid(frame);
        noia_event_signal_emit_int(SIGNAL_KEYBOARD_FOCUS_CHANGED, sid);
    }
}

//------------------------------------------------------------------------------

NoiaFrame* noia_compositor_create_new_workspace(NoiaCompositor* self,
                                                NoiaSize size)
{
    NOIA_ENSURE(self, return NULL);

    NoiaFrame* workspace = noia_frame_new();
    noia_frame_configure_as_workspace(workspace, size);
    noia_frame_append(self->root, workspace);

    /// @todo This should be configurable
    noia_compositor_set_selection(self, workspace);
    return workspace;
}

//------------------------------------------------------------------------------

void noia_compositor_log_frame(NoiaCompositor* self)
{
    noia_log_begin("FRAMES");
    noia_frame_log(self->root, noia_log_print, self->selection);
    noia_log_end();
}

//------------------------------------------------------------------------------

bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NOIA_ENSURE(self, return false);

    NoiaSurfaceData* surface = noia_surface_get(sid);
    if (!surface || !surface->is_toplevel) {
        return false;
    }

    /// @todo Frame type, size and position should be configurable.
    NoiaFrame* frame = noia_frame_new();
    noia_frame_configure(frame, NOIA_FRAME_TYPE_FLOATING | NOIA_FRAME_TYPE_LEAF,
                         sid, (NoiaPosition) {0,0}, surface->requested_size);

    noia_frame_append(self->selection, frame);
    /// @todo This should be configurable
    noia_compositor_set_selection(self, frame);

    noia_compositor_log_frame(self);

    return true;
}

//------------------------------------------------------------------------------

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NOIA_ENSURE(self, return);
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
    NOIA_ENSURE(self, return);

    // Pop in frame hierarchy.
    NoiaFrame* frame = noia_frame_find_with_sid(self->root, sid);
    noia_frame_pop_recursively(self->root, frame);

    // Pop in history.
    NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
    noia_list_remove(exhibitor->surface_history,
                    (void*) sid, (NoiaCompareFunc) noia_surface_compare);
    noia_list_prepend(exhibitor->surface_history, (void*) sid);

    // Update selection.
    noia_compositor_set_selection(self, frame);
}

//------------------------------------------------------------------------------

void noia_compositor_jump(NoiaCompositor* self NOIA_UNUSED,
                          NoiaFrame* frame     NOIA_UNUSED,
                          NoiaArgmand argmand  NOIA_UNUSED)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);
    NOIA_ENSURE(noia_argmand_is_directed(argmand), return);
}

//------------------------------------------------------------------------------

void noia_compositor_focus(NoiaCompositor* self,
                           NoiaArgmand argmand,
                           int position)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(noia_argmand_is_directed(argmand), return);

    if (argmand == NOIA_ARGMAND_BACK || argmand == NOIA_ARGMAND_FORWARD) {
        NoiaExhibitor* exhibitor = noia_exhibitor_get_instance();
        if (argmand == NOIA_ARGMAND_BACK) {
            position = -1 * position;
        }

        NoiaSurfaceId sid = (NoiaSurfaceId) noia_list_get_nth
                                         (exhibitor->surface_history, position);
        if (sid != scInvalidSurfaceId) {
            noia_compositor_pop_surface(self, sid);
        }
    } else {
        if (position < 0) {
            argmand = noia_argmand_reverse_directed(argmand);
            position = -position;
        }

        NoiaFrame* new_selection =
                   noia_frame_find_adjacent(self->selection, argmand, position);
        if (new_selection) {
            noia_compositor_set_selection(self, new_selection);
        }
    }

    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_configure(NoiaCompositor* self,
                               NoiaFrame* frame,
                               NoiaArgmand direction)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);
    NOIA_ENSURE(frame->trunk, return);

    // Translate direction to frame type
    NoiaFrameType type = NOIA_FRAME_TYPE_NONE;
    switch (direction) {
    case NOIA_ARGMAND_BEGIN:
    case NOIA_ARGMAND_END:
        type = NOIA_FRAME_TYPE_STACKED;
        break;

    case NOIA_ARGMAND_N:
    case NOIA_ARGMAND_S:
        type = NOIA_FRAME_TYPE_VERTICAL;
        break;

    case NOIA_ARGMAND_E:
    case NOIA_ARGMAND_W:
        type = NOIA_FRAME_TYPE_HORIZONTAL;
        break;

    default:
        break;
    }

    // Change frame type
    if (noia_frame_has_type(frame, NOIA_FRAME_TYPE_LEAF)) {
        /// @todo: Create new frame at trunk and resettle selection.
        noia_frame_change_type(frame->trunk, type);
    } else {
        noia_frame_change_type(frame, type);
    }

    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_anchorize(NoiaCompositor* self, NoiaFrame* frame)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);

    NOIA_BLOCK {
        if (!noia_frame_has_type(self->selection, NOIA_FRAME_TYPE_FLOATING)) {
            break;
        }

        NoiaFrame* workspace = noia_frame_find_top(self->selection);
        if (!workspace) {
            break;
        }

        noia_frame_resettle(frame, workspace);
    }
    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_execute(NoiaCompositor* self, NoiaAction* a)
{
    NOIA_ENSURE(a, return);
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(noia_argmand_is_actionable(a->action), return);
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
        noia_compositor_jump(self, self->selection, a->direction);
        break;
    case NOIA_ARGMAND_FOCUS:
        noia_compositor_focus(self, a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_CONF:
        noia_compositor_configure(self, self->selection, a->direction);
        break;
    case NOIA_ARGMAND_ANCHOR:
        noia_compositor_anchorize(self, self->selection);
        break;
    default: break;
    }
}

//------------------------------------------------------------------------------

