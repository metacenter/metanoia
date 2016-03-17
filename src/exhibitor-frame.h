// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_FRAME_H
#define NOIA_EXHIBITOR_FRAME_H

#include "utils-branch.h"
#include "utils-pool.h"

/// Structure representing frame containing surfaces and other frames.
typedef NoiaBranch NoiaFrame;

/// Enum type defining relative position to a frame.
typedef enum {
    NOIA_FRAME_POSITION_ON,
    NOIA_FRAME_POSITION_BEFORE,
    NOIA_FRAME_POSITION_AFTER,
} NoiaFramePosition;

/// `NoiaFrame` iterator.
typedef struct {
    unsigned num;
    NoiaFrame* frame;
    NoiaArgmand direction;
    NoiaFramePosition position;
} NoiaFrameIterator;

/// Create new frame.
NoiaFrame* noia_frame_new(void);

/// Frame destructor.
void noia_frame_free(NoiaFrame* self);

/// Configure frame parameters.
void noia_frame_configure(NoiaFrame* self,
                          NoiaFrameType type,
                          NoiaSurfaceId sid,
                          NoiaPosition pos,
                          NoiaSize size,
                          const char* title);

/// Configure frame to be a workspace.
void noia_frame_configure_as_workspace(NoiaFrame* self,
                                       NoiaSize size,
                                       const char* title);

/// Append self and all children surface context to array `surfaces`.
/// @see noia_display_redraw_all
void noia_frame_to_array(NoiaFrame* self, NoiaPool* surfaces);

/// Check if frame has compatible type.
bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type);

/// Get frame title.
const char* noia_frame_get_title(NoiaFrame* self);

/// Get frame surface ID.
NoiaSurfaceId noia_frame_get_sid(NoiaFrame* self);

/// Get frame area.
NoiaArea noia_frame_get_area(NoiaFrame* self);

/// Swap surface ID of given frames.
/// This does not rearrange frames.
NoiaResult noia_frame_swap(NoiaFrame* self, NoiaFrame* frame);

/// Resize the frame.
void noia_frame_resize(NoiaFrame* self,
                       NoiaArgmand border,
                       int magnitude);

/// Move the frame.
/// This takes effect only on surfaces which are FLOATING but not FIXED.
void noia_frame_move(NoiaFrame* self,
                     NoiaArgmand direction,
                     int magnitude);

/// Change directed type of frame.
/// @todo Add unit tests.
NoiaResult noia_frame_change_type(NoiaFrame* self, NoiaFrameType type);

/// Pop the surface `pop` and its parents recursively ending on `self`.
void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop);

/// Detach frame `self` from its trunk.
NoiaResult noia_frame_remove_self(NoiaFrame* self);

/// Remove frame `self` from its current trunk and insert to frame `target`.
/// @todo Add unit tests.
NoiaResult noia_frame_jump(NoiaFrame* self, NoiaFrame* target);

/// Insert frame `self` in frame `target`.
/// @todo Add unit tests.
NoiaResult noia_frame_jumpin(NoiaFrame* self, NoiaFrame* target);

/// Find a frame holding surface with given ID.
NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid);

/// Find leaf frame contained in frame `self` containing `point` or the closest
/// one if `point` lies outside `self`.
NoiaFrame* noia_frame_find_pointed(NoiaFrame* self, NoiaPosition point);

/// Find top-most frame bordering with frame `self` in given direction.
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

/// Initialize iterator.
void noia_frame_start_iteration(NoiaFrameIterator* iter,
                                NoiaFrame* frame,
                                NoiaArgmand direction);

/// Iterate through Frames.
void noia_frame_iterate(NoiaFrameIterator* iter);

/// Print frame tree to log file.
void noia_frame_log(NoiaFrame* self, NoiaPrintFunc print, NoiaFrame* selection);

#endif // NOIA_EXHIBITOR_FRAME_H

