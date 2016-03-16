// file: exhibitor-frame-internal.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_FRAME_INTERNAL_H
#define NOIA_EXHIBITOR_FRAME_INTERNAL_H

#include "exhibitor-frame.h"

/// Parameters describing state of surface hold by frame.
typedef struct {
    /// Type of the frame.
    NoiaFrameType type;

    /// Surface ID.
    NoiaSurfaceId sid;

    /// Position in workspace coordinates and size of the frame.
    NoiaArea area;

    /// Title if frame; ID of workspace.
    char* title;
} NoiaFrameParams;

/// Get the parameters - convenience function.
static inline NoiaFrameParams* noia_frame_get_params(NoiaFrame* self)
{
    return self->base.data;
}

/// Parameters destructor.
void noia_frame_params_free(NoiaFrameParams* params);

/// Compare frame parameters.
/// @return True if parameters are equivalent, else false.
bool noia_frame_parameters_are_equivalent(NoiaFrameParams* p1,
                                          NoiaFrameParams* p2);

/// Compare function - check if given params have give surface ID.
int noia_frame_params_compare_sid(NoiaFrameParams* params, NoiaSurfaceId sid);

/// Set surface ID.
void noia_frame_set_surface(NoiaFrame* self, NoiaSurfaceId sid);

/// Set frame size.
void noia_frame_set_size(NoiaFrame* self, NoiaSize size);

/// Set frame position.
void noia_frame_set_position(NoiaFrame* self, NoiaPosition position);

/// Add child frame to ending.
/// If `self` has twigs add `other` to them.
/// If not, add to subframes of its trunk.
void noia_frame_append(NoiaFrame* self, NoiaFrame* other);

/// Add child frame to beginning.
/// If `self` has twigs add `other` to them.
/// If not, add to subframes of its trunk.
void noia_frame_prepend(NoiaFrame* self, NoiaFrame* other);

/// Insert `other` to twigs of `self`'s trunk just before `self`.
static inline void noia_frame_insert_before(NoiaFrame* self, NoiaFrame* other)
{
    noia_branch_insert_before(self, other);
}

/// Insert `other` to twigs of `self`'s trunk just after `self`.
static inline void noia_frame_insert_after(NoiaFrame* self, NoiaFrame* other)
{
    noia_branch_insert_after(self, other);
}

/// Remove frame `self` from its current trunk and prepend to frame `target`.
NoiaResult noia_frame_resettle(NoiaFrame* self, NoiaFrame* target);

/// @see noia_frame_resize
NoiaFrame* noia_frame_find_trunk_with_type(NoiaFrame* frame,
                                           NoiaFrameType type);

/// Resize the surface frame is holding.
void noia_frame_reconfigure(NoiaFrame* self, NoiaArea area);

/// Resize anchored frame.
void noia_frame_resize_anchored(NoiaFrame* self,
                                NoiaArgmand border,
                                int magnitude);

/// Resize floating frame.
void noia_frame_resize_floating(NoiaFrame* self,
                                NoiaArgmand border,
                                int magnitude);

/// Move the frame with all twigs.
void noia_frame_move_with_contents(NoiaFrame* self, NoiaPosition vector);

/// Helper function for log printing.
/// @see noia_frame_log
void noia_frame_log_internal(NoiaFrame* self,
                             NoiaPrintFunc print,
                             NoiaFrame* selection,
                             unsigned level);

#endif // NOIA_EXHIBITOR_FRAME_INTERNAL_H

