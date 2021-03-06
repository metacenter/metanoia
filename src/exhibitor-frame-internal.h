// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_FRAME_INTERNAL_H
#define NOIA_EXHIBITOR_FRAME_INTERNAL_H

#include "utils-branch.h"
#include "exhibitor-frame.h"
#include "surface-coordinator.h"

/// Parameters describing state of surface hold by frame.
struct NoiaFrame {
    /// Branch.
    NoiaBranch branch;

    /// Type of the frame.
    NoiaFrameType type;

    /// Surface ID.
    NoiaSurfaceId sid;

    /// Position in workspace coordinates and size of the frame.
    NoiaArea area;

    /// Title if frame; ID of workspace.
    char* title;
};

//------------------------------------------------------------------------------

/// Compare `sid` inside `frame` with given sid.
int noia_frame_compare_sid(NoiaFrame* frame, NoiaSurfaceId sid);

//------------------------------------------------------------------------------

/// Set surface ID and inform surface if necessary.
void noia_frame_set_surface(NoiaFrame* self,
                            NoiaCoordinator* coordinator,
                            NoiaSurfaceId sid);

/// Set frame size and inform surface if necessary.
void noia_frame_set_size(NoiaFrame* self,
                         NoiaCoordinator* coordinator,
                         NoiaSize size);

/// Set frame position and inform surface if necessary.
void noia_frame_set_position(NoiaFrame* self, NoiaPosition position);

//------------------------------------------------------------------------------

/// @see noia_frame_resize
NoiaFrame* noia_frame_find_trunk_with_type(NoiaFrame* frame,
                                           NoiaFrameType type);

//------------------------------------------------------------------------------

/// Add child frame to ending.
/// If `self` has twigs add `other` to them.
/// If not, add to subframes of its trunk.
/// @return frame to which `other` was appended.
NoiaFrame* noia_frame_append(NoiaFrame* self, NoiaFrame* other);

/// Add child frame to beginning.
/// If `self` has twigs add `other` to them.
/// If not, add to subframes of its trunk.
/// @return frame to which `other` was prepended.
NoiaFrame* noia_frame_prepend(NoiaFrame* self, NoiaFrame* other);

/// Insert `other` to twigs of `self`'s trunk just before `self`.
static inline void noia_frame_insert_before(NoiaFrame* self, NoiaFrame* other)
{
    noia_branch_insert_before(&self->branch, &other->branch);
}

/// Insert `other` to twigs of `self`'s trunk just after `self`.
static inline void noia_frame_insert_after(NoiaFrame* self, NoiaFrame* other)
{
    noia_branch_insert_after(&self->branch, &other->branch);
}

//------------------------------------------------------------------------------

/// Resize the surface frame is holding.
void noia_frame_reconfigure(NoiaFrame* self,
                            NoiaCoordinator* coordinator,
                            NoiaArea area);

/// Resize anchored frame.
void noia_frame_resize_anchored(NoiaFrame* self,
                                NoiaCoordinator* coordinator,
                                NoiaDirection border,
                                int magnitude);

/// Resize floating frame.
void noia_frame_resize_floating(NoiaFrame* self,
                                NoiaCoordinator* coordinator,
                                NoiaDirection border,
                                int magnitude);

/// Make all subsurfaces have the same size and proper layout.
/// Relaxing works only on directed frames.
void noia_frame_relax(NoiaFrame* self, NoiaCoordinator* coordinator);

/// Move the frame with all twigs.
void noia_frame_move_with_contents(NoiaFrame* self, NoiaPosition vector);

//------------------------------------------------------------------------------

/// Helper function for log printing.
/// @see noia_frame_log
void noia_frame_log_internal(NoiaFrame* self,
                             NoiaPrintFunc print,
                             NoiaFrame* selection,
                             unsigned level);

//------------------------------------------------------------------------------

#endif // NOIA_EXHIBITOR_FRAME_INTERNAL_H

