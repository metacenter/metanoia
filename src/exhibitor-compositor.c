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

/// Search for existing workspace with given title.
NoiaFrame* noia_compositor_find_current_workspace(NoiaCompositor* self)
{
    return noia_frame_find_top(self->selection)->trunk;
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
    NoiaFrame* selection = workspace;
    noia_frame_configure(workspace, self->coordinator,
                         NOIA_FRAME_TYPE_WORKSPACE, scInvalidSurfaceId,
                         noia_frame_get_area(display), title);
    noia_frame_settle(workspace, display, self->coordinator);

    // Create subframe depending on configuration
    if (noia_config()->default_frame_type != NOIA_FRAME_TYPE_NONE) {
        selection = noia_frame_new();
        noia_frame_configure(selection, self->coordinator,
                             noia_config()->default_frame_type, 0,
                             noia_frame_get_area(display), NULL);
        noia_frame_settle(selection, workspace, self->coordinator);
    }

    /// @todo Focusing new workspace should be configurable
    noia_compositor_set_selection(self, selection);
    noia_frame_pop_recursively(self->root, selection);
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
        NoiaFrame* current = noia_compositor_find_current_workspace(self);
        NOIA_ENSURE(current, break);
        NoiaFrame* display_frame = noia_frame_get_trunk(current);
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
    noia_frame_configure(self->root, self->coordinator, NOIA_FRAME_TYPE_SPECIAL,
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
    noia_frame_settle(display, self->root, self->coordinator);
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

    LOG_INFO2("Manage surface %u", sid);

    NoiaFrame* target = noia_frame_buildable(self->selection);
    NoiaArea area = {surface->requested_position, surface->requested_size};
    NoiaFrameType type =
      (noia_frame_has_type(target, NOIA_FRAME_TYPE_DIRECTED)
      ? NOIA_FRAME_TYPE_NONE : NOIA_FRAME_TYPE_FLOATING) | NOIA_FRAME_TYPE_LEAF;

    // Create and glue new frame
    NoiaFrame* frame = noia_frame_new();
    noia_frame_configure(frame, self->coordinator, type, sid, area, NULL);
    noia_frame_settle(frame, target, self->coordinator);
    /// @todo This should be configurable
    noia_compositor_set_selection(self, frame);

    return true;
}

//------------------------------------------------------------------------------

void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid)
{
    NOIA_ENSURE(self, return);

    NoiaFrame* frame = noia_frame_find_with_sid(self->root, sid);
    if (frame) {
        LOG_INFO2("Unmanage surface %u", sid);

        if (frame == self->selection) {
            /// @todo This should be configurable
            noia_compositor_set_selection(self, NULL);
        }

        noia_frame_remove_self(frame, self->coordinator);
        noia_frame_free(frame);

        noia_coordinator_notify(self->coordinator);
        noia_compositor_log_frame(self);
    }
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

void noia_compositor_ramify(NoiaCompositor* self, NoiaFrame* frame)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);

    NoiaFrame* new_frame = noia_frame_ramify(frame,
                                             noia_config()->default_frame_type,
                                             self->coordinator);
    noia_compositor_set_selection(self, new_frame);

    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_enlarge(NoiaCompositor* self, NoiaFrame* frame)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);

    NoiaFrame* new_frame = NULL;
    NoiaFrame* trunk = noia_frame_get_trunk(frame);
    if (noia_frame_has_type(trunk, NOIA_FRAME_TYPE_STACKED)) {
        NoiaFrame* top = noia_frame_get_trunk(trunk);
        if (noia_frame_has_type(top, NOIA_FRAME_TYPE_STACKED)) {
            noia_frame_resettle(frame, top, self->coordinator);
        } else {
            new_frame = noia_frame_ramify(top,
                                          NOIA_FRAME_TYPE_STACKED,
                                          self->coordinator);
            noia_frame_resettle(frame, new_frame, self->coordinator);
        }
    } else {
        new_frame = noia_frame_ramify(trunk,
                                      NOIA_FRAME_TYPE_STACKED,
                                      self->coordinator);
        noia_frame_resettle(frame, new_frame, self->coordinator);
    }

    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_jump(NoiaCompositor* self,
                          NoiaFrame* frame,
                          NoiaDirection direction,
                          int distance)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);

    LOG_INFO2("Compositor: jump");

    // Modify direction if needed
    if (distance < 0) {
        direction = noia_direction_reverse(direction);
        distance = -distance;
    }

    // Calculate position
    NoiaFramePosition position = NOIA_FRAME_POSITION_ON;
    if ((direction == NOIA_DIRECTION_N)
    or  (direction == NOIA_DIRECTION_W)) {
        position = NOIA_FRAME_POSITION_BEFORE;
    } else {
        position = NOIA_FRAME_POSITION_AFTER;
    }

    // Find target
    NoiaFrame* target = noia_frame_find_adjacent(frame, direction, distance);
    if (not target) {
        NoiaFrameType type = noia_direction_translate_to_frame_type(direction);
        target = noia_frame_find_top(frame);
        noia_frame_ramify(target, type, self->coordinator);
    }

    // Perform jump
    NoiaFrame* source = noia_frame_get_trunk(frame);
    noia_frame_jump(frame, position, target, self->coordinator);
    noia_frame_deramify(source);

    // Log result
    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_dive(NoiaCompositor* self,
                          NoiaFrame* frame,
                          NoiaDirection direction,
                          int position)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);

    LOG_INFO2("Compositor: jump");

    // Modify direction if needed
    if (position < 0) {
        direction = noia_direction_reverse(direction);
        position = -position;
    }

    // Perform jump
    NoiaFrame* target = noia_frame_find_adjacent(frame, direction, position);
    if (target) {
        NoiaFrame* source = noia_frame_get_trunk(frame);
        noia_frame_jump(frame,
                        NOIA_FRAME_POSITION_ON,
                        target,
                        self->coordinator);
        noia_frame_deramify(source);
    }

    // Log result
    noia_compositor_log_frame(self);
}
//------------------------------------------------------------------------------

void noia_compositor_jump_to_workspace(NoiaCompositor* self, char* title)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(title, return);

    LOG_INFO2("Compositor: jump to workspace '%s'", title);

    NoiaFrame* workspace = noia_compositor_bring_workspace(self, title);
    if (workspace) {
        noia_frame_jump(self->selection,
                        NOIA_FRAME_POSITION_ON,
                        workspace,
                        self->coordinator);
    } else {
        LOG_WARN1("Compositor: Workspace '%s' not found "
                  "and could not be created!", title);
    }
}

//------------------------------------------------------------------------------

void noia_compositor_focus(NoiaCompositor* self,
                           NoiaFrame* frame,
                           NoiaDirection direction,
                           int position)
{
    NOIA_ENSURE(self, return);

    LOG_INFO2("Compositor: focus");

    if ((direction == NOIA_DIRECTION_BACK)
    or  (direction == NOIA_DIRECTION_FORWARD)) {
        if (direction == NOIA_DIRECTION_FORWARD) {
            position = -1 * position;
        }

        NoiaList* history = noia_exhibitor_get_surface_history(self->exhibitor);
        NoiaSurfaceId sid = (NoiaSurfaceId)noia_list_get_nth(history, position);
        if (sid != scInvalidSurfaceId) {
            noia_compositor_pop_surface(self, sid);
        }
    } else {
        if (position < 0) {
            direction = noia_direction_reverse(direction);
            position = -position;
        }

        NoiaFrame* new_selection =
                           noia_frame_find_adjacent(frame, direction, position);
        if (new_selection) {
            noia_compositor_set_selection(self, new_selection);
        }
    }

    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_swap(NoiaCompositor* self,
                          NoiaFrame* frame,
                          NoiaDirection direction,
                          int position)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);

    if (position < 0) {
        direction = noia_direction_reverse(direction);
        position = -position;
    }

    NoiaFrame* target = noia_frame_find_adjacent(frame, direction, position);
    if (target) {
        noia_frame_swap(frame, target, self->coordinator);
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
                               NoiaDirection direction)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(frame, return);
    NOIA_ENSURE(frame->trunk, return);

    // Translate direction to frame type
    NoiaFrameType type = noia_direction_translate_to_frame_type(direction);

    LOG_INFO2("Compositor: Change frame type to 0x%04x", type);

    // Change frame type
    if (noia_frame_has_type(frame, NOIA_FRAME_TYPE_LEAF)) {
        /// @todo Create new frame at trunk and resettle selection.
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

        NoiaFrame* workspace = noia_compositor_find_current_workspace(self);
        if (not workspace) {
            break;
        }

        noia_frame_jump(frame,
                        NOIA_FRAME_POSITION_ON,
                        workspace,
                        self->coordinator);
    }
    noia_compositor_log_frame(self);
}

//------------------------------------------------------------------------------

void noia_compositor_execute(NoiaCompositor* self, NoiaCommand* c)
{
    NOIA_ENSURE(c, return);
    NOIA_ENSURE(self, return);

    if (self->selection) {
        switch (c->action) {
        case NOIA_ACTION_RESIZE:
            noia_frame_resize(self->selection, self->coordinator,
                              c->direction, c->magnitude);
            break;
        case NOIA_ACTION_MOVE:
            noia_frame_move(self->selection, c->direction, c->magnitude);
            break;
        case NOIA_ACTION_JUMP:
            if (c->direction == NOIA_DIRECTION_WORKSPACE) {
                noia_compositor_jump_to_workspace(self, c->str);
            } else if (c->direction == NOIA_DIRECTION_END) {
                noia_compositor_ramify(self, self->selection);
            } else if (c->direction == NOIA_DIRECTION_BEGIN) {
                noia_compositor_enlarge(self, self->selection);
            } else {
                noia_compositor_jump(self, self->selection,
                                     c->direction, c->magnitude);
            }
            break;
        case NOIA_ACTION_DIVE:
            noia_compositor_dive(self, self->selection,
                                 c->direction, c->magnitude);
            break;
        case NOIA_ACTION_FOCUS:
            if (c->direction == NOIA_DIRECTION_WORKSPACE) {
                noia_compositor_focus_workspace(self, c->str);
            } else {
                noia_compositor_focus(self, self->selection,
                                      c->direction, c->magnitude);
            }
            break;
        case NOIA_ACTION_SWAP:
            noia_compositor_swap(self, self->selection,
                                 c->direction, c->magnitude);
            break;
        case NOIA_ACTION_CONF:
            noia_compositor_configure(self, self->selection, c->direction);
            break;
        case NOIA_ACTION_ANCHOR:
            noia_compositor_anchorize(self, self->selection);
            break;
        default: break;
        }
    }

    noia_coordinator_notify(self->coordinator);
}

//------------------------------------------------------------------------------

