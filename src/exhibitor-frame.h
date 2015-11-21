// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_FRAME_H__
#define __NOIA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"
#include "utils-list.h"
#include "utils-pool.h"

/// @file
/// @todo Unit tests for NoiaFrame.

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

    /// Position in workspace coordinates.
    NoiaPosition pos;

    /// Size of the frame.
    NoiaSize size;
} NoiaFrameParams;

/// Compare frame parameters.
/// @return True if parameters are equivalent, else false.
bool noia_frame_parameters_are_equivalent(NoiaFrameParams* p1,
                                          NoiaFrameParams* p2);

/// Create new frame.
NoiaFrame* noia_frame_new();

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

/// Add child frame to ending.
void noia_frame_append(NoiaFrame* self, NoiaFrame* other);

/// Add child frame to beginning.
void noia_frame_prepend(NoiaFrame* self, NoiaFrame* other);

/// Find first trunk which type has NOIA_FRAME_TYPE_SPECIAL.
/// For normal frame this should be workspace.
NoiaFrame* noia_frame_get_top(NoiaFrame* self);

/// Remove frame `self` from its current parent and prepend to frame `target`.
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

/// @todo Implement noia_frame_jump
void noia_frame_jump(NoiaFrame* self,
                     NoiaArgmand direction,
                     int magnitude);

/// Pop the surface `pop` and its parents recursively ending on `self`.
void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop);

/// Detach frame `self` from its parent.
NoiaResult noia_frame_remove_self(NoiaFrame* self);

/// Find a frame holding surface with given ID.
NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid);

/// Find a frame `magnitude` frames forther in `direction` direction.
NoiaFrame* noia_frame_find_pointed(NoiaFrame* self,
                                   NoiaArgmand direction,
                                   int magnitude);

/// Print frame tree to log file.
void noia_frame_log(NoiaFrame* self);

#endif // __NOIA_EXHIBITOR_FRAME_H__

