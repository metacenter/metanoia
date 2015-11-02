// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_FRAME_H__
#define __NOIA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"
#include "utils-list.h"
#include "utils-pool.h"

/// @file
/// @todo Unit tests for NoiaFrame.

/// Frame types.
typedef enum {
    NOIA_FRAME_TYPE_NONE       = 0x00,
    NOIA_FRAME_TYPE_STACKED    = 0x01,
    NOIA_FRAME_TYPE_HORIZONTAL = 0x02,
    NOIA_FRAME_TYPE_VERTICAL   = 0x04,
    NOIA_FRAME_TYPE_FLOATING   = 0x08,
    NOIA_FRAME_TYPE_FIXED      = 0x10,
    NOIA_FRAME_TYPE_LEAF       = 0x20,

    NOIA_FRAME_TYPE_WORKSPACE  = 0x1000 | NOIA_FRAME_TYPE_FIXED
                                        | NOIA_FRAME_TYPE_STACKED,
} NoiaFrameType;

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

/// Create new frame.
NoiaFrame* noia_frame_create(NoiaPosition position, NoiaSize size);

/// Create new frame with the same position and size as `self`, attach to `self`
/// and set new type.
/// @note This function only takes effect if frames type is STACKED.
NoiaFrame* noia_frame_create_child(NoiaFrame* self, NoiaFrameType type);

/// Frame destructor.
void noia_frame_free(NoiaFrame* self);

/// Configure frame to be a workspace.
void noia_frame_init_as_workspace(NoiaFrame* self);

/// Append self and all children surface context to array `surfaces`.
/// @see noia_display_redraw_all
void noia_frame_to_array(NoiaFrame* self, NoiaPool* surfaces);

/// Set surface ID.
void noia_frame_set_surface(NoiaFrame* self, NoiaSurfaceId sid);

/// Set surface type.
void noia_frame_set_type(NoiaFrame* self, NoiaFrameType type);

/// Check if frame has compatible type.
bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type);

/// Add child frame to ending.
void noia_frame_append(NoiaFrame* self, NoiaFrame* other);

/// Add child frame to beginning.
void noia_frame_prepend(NoiaFrame* self, NoiaFrame* other);

/// Remove frame `self` from its current parent and prepend to frame `target`.
NoiaResult noia_frame_resettle(NoiaFrame* self, NoiaFrame* target);

/// Resize the frame.
void noia_frame_resize(NoiaFrame* self,
                       NoiaArgmandType direction,
                       int magnitude);

/// Move the frame.
/// This takes effect only on surfaces which are FLOATING but not FIXED.
void noia_frame_move(NoiaFrame* self,
                     NoiaArgmandType direction,
                     int magnitude);

/// @todo Implement noia_frame_jump
void noia_frame_jump(NoiaFrame* self,
                     NoiaArgmandType direction,
                     int magnitude);

/// Pop the surface `pop` and its parents recursively ending on `self`.
void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop);

/// Detach frame `self` from its parent.
NoiaResult noia_frame_remove_self(NoiaFrame* self);

/// Find a frame holding surface with given ID.
NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid);

/// Print frame tree to log file.
void noia_frame_log(NoiaFrame* self);

#endif // __NOIA_EXHIBITOR_FRAME_H__

