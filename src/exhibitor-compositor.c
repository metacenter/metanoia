// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-compositor.h"

#include "surface-coordinator.h"
#include "event-signals.h"
#include "utils-log.h"
#include "config.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------
// PRIVATE

struct NoiaCompositorStruct {
    NoiaFrame* root;
    NoiaFrame* selection;
    NoiaExhibitor* exhibitor;
    NoiaCoordinator* coordinator;
};

//------------------------------------------------------------------------------

/// Set given frame as selection.
void noia_compositor_set_selection(NoiaCompositor* self, NoiaFrame* frame)
{
    NOIA_ENSURE(self, return);

    NoiaSurfaceId sid = scInvalidSurfaceId;

    if (not frame) {
        if (self->selection
        and noia_frame_has_type(self->selection, NOIA_FRAME_TYPE_LEAF)) {
            self->selection = self->selection->trunk;
        }
    } else if (noia_frame_has_type(frame, NOIA_FRAME_TYPE_LEAF)) {
        sid = noia_frame_get_sid(frame);
        self->selection = frame;
    } else {
        self->selection = frame;
    }

    if (sid != scInvalidSurfaceId) {
        noia_surface_set_focus(self->coordinator, sid);
    }
}

//------------------------------------------------------------------------------

/// Search for existing workspace with given title.
NoiaFrame* noia_compositor_find_workspace(NoiaCompositor* self,
                                          const char* title)
{
    NoiaFrame* result = NULL;
    FOR_EACH_TWIG(self->root, output_frame) {
        FOR_EACH_TWIG(output_frame, workspace) {
            if (strcmp(title, noia_frame_get_title(workspace)) == 0) {
                result = workspace;
                break;
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------

/// Creates new frame, places it in proper place in frame tree and initializes
/// it as a workspace.
NoiaFrame* noia_compositor_create_new_workspace(NoiaCompositor* self,
                                                NoiaFrame* display,
                                                const char* title)
{
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(title, return NULL);
    NOIA_ENSURE(strlen(title) > 0, return NULL);

    // Create and configure workspace
    NoiaFrame* workspace = noia_frame_new();
    NoiaFrameType type = NOIA_FRAME_TYPE_WORKSPACE
                       | noia_config()->workspace_type;

    noia_frame_configure(workspace, self->coordinator, type, 0,
                         noia_frame_get_area(display), title);
    noia_frame_jumpin(display, workspace, self->coordinator);

    /// @todo Focusing new workspace should be configurable
    noia_compositor_set_selection(self, workspace);
    noia_frame_pop_recursively(self->root, workspace);
    return workspace;
}

//------------------------------------------------------------------------------

/// Search for existing workspace or create new with given title.
NoiaFrame* noia_compositor_bring_workspace(NoiaCompositor* self,
                                           const char* title)
{
    NoiaFrame* workspace = NULL;
    NOIA_BLOCK {
        workspace = noia_compositor_find_workspace(self, title);
        if (workspace) {
            break;
        }

        /// @todo For many output setup this should be cofigurable on which
        ///       output the workspace will be created.
        NoiaFrame* current_workspace = noia_frame_find_top(self->selection);
        NOIA_ENSURE(current_workspace, break);
        NoiaFrame* display_frame = noia_frame_get_parent(current_workspace);
        NOIA_ENSURE(display_frame, break);

        workspace = noia_compositor_create_new_workspace
                                                   (self, display_frame, title);
    }
    return workspace;
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaCompositor* noia_compositor_new(NoiaExhibitor* exhibitor)
{
    NoiaCompositor* self = malloc(sizeof(NoiaCompositor));
    NOIA_ENSURE(self, abort());

    NoiaArea area = {{0, 0}, {-1, -1}};
    self->exhibitor = exhibitor;
    self->coordinator = noia_exhibitor_get_coordinator(exhibitor);
    self->root = noia_frame_new();
    noia_frame_configure(self->root, self->coordinator, NOIA_FRAME_TYPE_STACKED,
                         scInvalidSurfaceId, area, "METANOIA");
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

NoiaFrame* noia_compositor_create_new_display(NoiaCompositor* self,
                                              NoiaArea area,
                                              const char* title)
{
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(title, return NULL);
    NOIA_ENSURE(strlen(title) > 0, return NULL);

    // Create and configure output frame
    NoiaFrame* display = noia_frame_new();
    noia_frame_configure(display, self->coordinator,
                         NOIA_FRAME_TYPE_DISPLAY, 0, area, title);
    noia_frame_jumpin(self->root, display, self->coordinator);
    return display;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_compositor_create_next_workspace(NoiaCompositor* self,
                                                 NoiaFrame* display)
{
    NOIA_ENSURE(self, return NULL);

    // Generate title for workspace
    char title[4] = {'\0'};
    for (int i = 1; i < 1000; ++i) {
        snprintf(title, sizeof(title), "%d", i);
        if (not noia_compositor_find_workspace(self, title)) {
            break;
        }
        title[0] = '\0';
    }

    // Create and configure workspace
    return noia_compositor_create_new_workspace(self, display, title);
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

    NoiaSurfaceData* surface = noia_surface_get(self->coordinator, sid);
    NOIA_ENSURE(surface, return false);

    NoiaFrame* target = noia_frame_buildable(self->selection);

    NoiaFrameType type =
      (noia_frame_has_type(target, NOIA_FRAME_TYPE_DIRECTED)
      ? NOIA_FRAME_TYPE_NONE : NOIA_FRAME_TYPE_FLOATING) | NOIA_FRAME_TYPE_LEAF;

    NoiaFrame* frame = noia_frame_new();
    NoiaArea area = {{0,0}, surface->requested_size};
    noia_frame_configure(frame, self->coordinator, type, sid, area, NULL);

    noia_frame_jumpin(target, frame, self->coordinator);
    /// @todo This should be configurable
    noia_compositor_set_selection(self, frame);

    noia_compositor_log_frame(self);

    return true;
}

//------------------------------------------------------------------------------

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NOIA_ENSURE(self, return);

    /// @todo This should be configurable
    noia_compositor_set_selection(self, NULL);

    NoiaFrame* frame = noia_frame_find_with_sid(self->root, sid);
    noia_frame_remove_self(frame);
    noia_frame_free(frame);

    noia_compositor_log_frame(self);
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
    NoiaList* history = noia_exhibitor_get_surface_history(self->exhibitor);
    noia_list_remove(history, (void*) sid,
                     (NoiaCompareFunc) noia_surface_compare);
    noia_list_prepend(history, (void*) sid);

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

void noia_compositor_jump_to_workspace(NoiaCompositor* self, char* title)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(title, return);

    NoiaFrame* workspace = noia_compositor_bring_workspace(self, title);
    if (workspace) {
        noia_frame_jump(self->selection, workspace, self->coordinator);
    } else {
        LOG_WARN1("Compositor: Workspace '%s' not found "
                  "and could not be created!", title);
    }
}

//------------------------------------------------------------------------------

void noia_compositor_focus(NoiaCompositor* self,
                           NoiaArgmand argmand,
                           int position)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(noia_argmand_is_directed(argmand), return);

    if ((argmand == NOIA_ARGMAND_BACK) or (argmand == NOIA_ARGMAND_FORWARD)) {
        if (argmand == NOIA_ARGMAND_BACK) {
            position = -1 * position;
        }

        NoiaList* history = noia_exhibitor_get_surface_history(self->exhibitor);
        NoiaSurfaceId sid = (NoiaSurfaceId)noia_list_get_nth(history, position);
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

void noia_compositor_swap(NoiaCompositor* self,
                          NoiaArgmand argmand,
                          int position)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(noia_argmand_is_directed(argmand), return);

    if (position < 0) {
        argmand = noia_argmand_reverse_directed(argmand);
        position = -position;
    }

    if (self->selection) {
        NoiaFrame* frame =
                   noia_frame_find_adjacent(self->selection, argmand, position);
        if (frame) {
            noia_frame_swap(self->selection, frame, self->coordinator);
        }
    }
}

//------------------------------------------------------------------------------

void noia_compositor_focus_workspace(NoiaCompositor* self, char* title)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(title, return);

    LOG_INFO1("Compositor: Change workspace to '%s'", title);

    NoiaFrame* workspace = noia_compositor_bring_workspace(self, title);
    if (not workspace) {
        LOG_WARN1("Compositor: Workspace '%s' not found!", title);
        return;
    }

    /// Searching for new selection is done by iterating through surface history
    /// and checking if surface with given ID is somewhere in workspace three.
    /// Not the most efficient...
    /// Any ideas for improvement?
    NoiaFrame* frame = NULL;
    NoiaList* history = noia_exhibitor_get_surface_history(self->exhibitor);
    FOR_EACH(history, link) {
        NoiaSurfaceId sid = (NoiaSurfaceId) link->data;
        frame = noia_frame_find_with_sid(workspace, sid);
        if (frame) {
            break;
        }
    }

    noia_frame_pop_recursively(self->root, workspace);
    self->selection = (frame ? frame : workspace);
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

    LOG_INFO2("Compositor: Change frame type to 0x%04x", type);

    // Change frame type
    if (noia_frame_has_type(frame, NOIA_FRAME_TYPE_LEAF)) {
        /// @todo: Create new frame at trunk and resettle selection.
        noia_frame_change_type(frame->trunk, self->coordinator, type);
    } else {
        noia_frame_change_type(frame, self->coordinator, type);
    }

    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_anchorize(NoiaCompositor* self, NoiaFrame* frame)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);

    NOIA_BLOCK {
        if (not noia_frame_has_type(self->selection, NOIA_FRAME_TYPE_FLOATING)){
            break;
        }

        NoiaFrame* workspace = noia_frame_find_top(self->selection);
        if (not workspace) {
            break;
        }

        noia_frame_jump(frame, workspace, self->coordinator);
    }
    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_execute(NoiaCompositor* self, NoiaAction* a)
{
    NOIA_ENSURE(a, return);
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(noia_argmand_is_actionable(a->action), return);
    if (not self->selection) {
        return;
    }

    switch (a->action) {
    case NOIA_ARGMAND_RESIZE:
        noia_frame_resize(self->selection, self->coordinator,
                          a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_MOVE:
        noia_frame_move(self->selection, a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_JUMP:
        if (a->direction == NOIA_ARGMAND_WORKSPACE) {
            noia_compositor_jump_to_workspace(self, a->str);
        } else {
            noia_compositor_jump(self, self->selection, a->direction);
        }
        break;
    case NOIA_ARGMAND_FOCUS:
        if (a->direction == NOIA_ARGMAND_WORKSPACE) {
            noia_compositor_focus_workspace(self, a->str);
        } else {
            noia_compositor_focus(self, a->direction, a->magnitude);
        }
        break;
    case NOIA_ARGMAND_SWAP:
        noia_compositor_swap(self, a->direction, a->magnitude);
        break;
    case NOIA_ARGMAND_CONF:
        noia_compositor_configure(self, self->selection, a->direction);
        break;
    case NOIA_ARGMAND_ANCHOR:
        noia_compositor_anchorize(self, self->selection);
        break;
    default: break;
    }

    noia_coordinator_notify(self->coordinator);
}

//------------------------------------------------------------------------------

