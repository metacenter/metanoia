// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_FRAME_H
#define NOIA_EXHIBITOR_FRAME_H

#include "utils-branch.h"
#include "utils-list.h"
#include "utils-pool.h"

/// @file
/// @todo Unit tests for NoiaFrame.

/// Structure representing frame containing surfaces and other frames.
typedef NoiaBranch NoiaFrame;

/// Parameters describing state of surface hold by frame.
typedef struct {
    /// Type of the frame.
    NoiaFrameType type;

    /// Unused.
    /// @todo Remove?
    char* title;

    /// Surface ID.
    NoiaSurfaceId sid;

    /// Position in workspace coordinates and size of the frame.
    NoiaArea area;
} NoiaFrameParams;

/// Compare frame parameters.
/// @return True if parameters are equivalent, else false.
bool noia_frame_parameters_are_equivalent(NoiaFrameParams* p1,
                                          NoiaFrameParams* p2);

/// Create new frame.
NoiaFrame* noia_frame_new(void);

/// Create new frame with the same position and size as `self`, attach to `self`
/// and set new type.
/// @note This function only takes effect if frames type is STACKED.
NoiaFrame* noia_frame_create_child(NoiaFrame* self, NoiaFrameType type);

/// Frame destructor.
void noia_frame_free(NoiaFrame* self);

/// Configure frame parameters.
void noia_frame_configure(NoiaFrame* self,
                          NoiaFrameType type,
                          NoiaSurfaceId sid,
                          NoiaPosition pos,
                          NoiaSize size);

/// Configure frame to be a workspace.
void noia_frame_configure_as_workspace(NoiaFrame* self,
                                       NoiaSize size);

/// Append self and all children surface context to array `surfaces`.
/// @see noia_display_redraw_all
void noia_frame_to_array(NoiaFrame* self, NoiaPool* surfaces);

/// Check if frame has compatible type.
bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type);

/// Get frame surface ID
NoiaSurfaceId noia_frame_get_sid(NoiaFrame* self);

/// Get frame area
NoiaArea noia_frame_get_area(NoiaFrame* self);

/// Add child frame to ending.
/// If `self` has twigs add `other` to them.
/// If not, add to subframes of its trunk.
void noia_frame_append(NoiaFrame* self, NoiaFrame* other);

/// Add child frame to beginning.
/// If `self` has twigs add `other` to them.
/// If not, add to subframes of its trunk.
void noia_frame_prepend(NoiaFrame* self, NoiaFrame* other);

/// Remove frame `self` from its current trunk and prepend to frame `target`.
NoiaResult noia_frame_resettle(NoiaFrame* self, NoiaFrame* target);

/// Resize the frame.
void noia_frame_resize(NoiaFrame* self,
                       NoiaArgmand direction,
                       int magnitude);

/// Move the frame.
/// This takes effect only on surfaces which are FLOATING but not FIXED.
void noia_frame_move(NoiaFrame* self,
                     NoiaArgmand direction,
                     int magnitude);

/// Change directed type of frame.
NoiaResult noia_frame_change_type(NoiaFrame* self, NoiaFrameType type);

/// Pop the surface `pop` and its parents recursively ending on `self`.
void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop);

/// Detach frame `self` from its trunk.
NoiaResult noia_frame_remove_self(NoiaFrame* self);

/// Find a frame holding surface with given ID.
NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid);

/// Find leaf frame contained in frame `self` containing `point` or the closest
/// one if `point` lies outside `self`.
NoiaFrame* noia_frame_find_pointed(NoiaFrame* self, NoiaPosition point);

/// Find find top-most frame bordering with frame `self` in given direction.
NoiaFrame* noia_frame_find_contiguous(NoiaFrame* self,
                                      NoiaArgmand direction,
                                      unsigned distance);

/// Find find bottom-most frame bordering with frame `self` in given direction.
NoiaFrame* noia_frame_find_adjacent(NoiaFrame* self,
                                    NoiaArgmand direction,
                                    unsigned distance);

/// Find first trunk which type has NOIA_FRAME_TYPE_SPECIAL.
/// For normal frame this should be workspace.
NoiaFrame* noia_frame_find_top(NoiaFrame* self);

/// Find first ancestor which trunk has type `type`, `FLOATING` or `SPECIAL`.
/// @see noia_frame_resize
NoiaFrame* noia_frame_find_trunk_with_type(NoiaFrame* frame,
                                           NoiaFrameType type);

/// Print frame tree to log file.
void noia_frame_log(NoiaFrame* self, NoiaPrintFunc print, NoiaFrame* selection);

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

#endif // NOIA_EXHIBITOR_FRAME_H

