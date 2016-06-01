// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_FRAME_H
#define NOIA_EXHIBITOR_FRAME_H

#include "utils-branch.h"
#include "utils-pool.h"
#include "surface-coordinator.h"

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
    NoiaDirection direction;
    NoiaFramePosition position;
} NoiaFrameIterator;

//------------------------------------------------------------------------------

/// Create new frame.
NoiaFrame* noia_frame_new(void);

/// Frame destructor.
void noia_frame_free(NoiaFrame* self);

/// Configure frame parameters.
void noia_frame_configure(NoiaFrame* self,
                          NoiaCoordinator* coordinator,
                          NoiaFrameType type,
                          NoiaSurfaceId sid,
                          NoiaArea area,
                          const char* title);

/// Append self and all children surface context to array `surfaces`.
/// @see noia_display_redraw_all
void noia_frame_to_array(NoiaFrame* self,
                         NoiaPool* surfaces,
                         NoiaCoordinator* coordinator);

//------------------------------------------------------------------------------

/// Check if frame has compatible type.
bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type);

/// Get frame title.
const char* noia_frame_get_title(NoiaFrame* self);

/// Get frame surface ID.
NoiaSurfaceId noia_frame_get_sid(NoiaFrame* self);

/// Get frame area.
NoiaArea noia_frame_get_area(NoiaFrame* self);

//------------------------------------------------------------------------------

/// Swap surface ID of given frames.
/// This does not rearrange frames.
NoiaResult noia_frame_swap(NoiaFrame* self,
                           NoiaFrame* frame,
                           NoiaCoordinator* coordinator);

/// Resize the frame.
void noia_frame_resize(NoiaFrame* self,
                       NoiaCoordinator* coordinator,
                       NoiaDirection border,
                       int magnitude);

/// Move the frame.
/// This takes effect only on surfaces which are FLOATING but not FIXED.
void noia_frame_move(NoiaFrame* self,
                     NoiaDirection direction,
                     int magnitude);

/// Change directed type of frame.
NoiaResult noia_frame_change_type(NoiaFrame* self,
                                  NoiaCoordinator* coordinator,
                                  NoiaFrameType type);

/// Pop the surface `pop` and its parents recursively ending on `self`.
void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop);

/// Detach frame `self` from its trunk.
NoiaResult noia_frame_remove_self(NoiaFrame* self,
                                  NoiaCoordinator* coordinator);

/// Insert frame `self` in frame `target`.
NoiaResult noia_frame_settle(NoiaFrame* self,
                             NoiaFrame* target,
                             NoiaCoordinator* coordinator);

/// Remove frame `self` from its current trunk and prepend to frame `target`.
NoiaResult noia_frame_resettle(NoiaFrame* self,
                               NoiaFrame* target,
                               NoiaCoordinator* coordinator);

/// Ramify given frame by creating new subframe and transfer configuration.
/// @return newly created frame.
NoiaFrame* noia_frame_ramify(NoiaFrame* self,
                             NoiaFrameType type,
                             NoiaCoordinator* coordinator);

/// If `self` has one non-leaf twig, resetle twigs twigs to self and remove
/// the twig.
void noia_frame_deramify(NoiaFrame* self);

/// Remove frame `self` from its current trunk and insert to frame `target`.
/// @todo Add more unit tests.
NoiaResult noia_frame_jump(NoiaFrame* self,
                           NoiaFramePosition position,
                           NoiaFrame* target,
                           NoiaCoordinator* coordinator);

/// Insert frame `self` in frame `target`.
NoiaResult noia_frame_jumpin(NoiaFrame* self,
                             NoiaFramePosition position,
                             NoiaFrame* target,
                             NoiaCoordinator* coordinator);

//------------------------------------------------------------------------------

/// Find a frame holding surface with given ID.
NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid);

/// Find leaf frame contained in frame `self` containing `point` or the closest
/// one if `point` lies outside `self`.
NoiaFrame* noia_frame_find_pointed(NoiaFrame* self, NoiaPosition point);

/// Find top-most frame bordering with frame `self` in given direction.
NoiaFrame* noia_frame_find_contiguous(NoiaFrame* self,
                                      NoiaDirection direction,
                                      unsigned distance);

/// Find find bottom-most frame bordering with frame `self` in given direction.
NoiaFrame* noia_frame_find_adjacent(NoiaFrame* self,
                                    NoiaDirection direction,
                                    unsigned distance);

//------------------------------------------------------------------------------

/// Find first trunk which type has NOIA_FRAME_TYPE_SPECIAL.
/// For normal frame this should be workspace.
NoiaFrame* noia_frame_find_top(NoiaFrame* self);

/// Find first trunk which type has not type NOIA_FRAME_TYPE_LEAF.
NoiaFrame* noia_frame_buildable(NoiaFrame* self);

/// Return parent of the frame.
NoiaFrame* noia_frame_get_trunk(NoiaFrame* self);

/// Return first subframe.
NoiaFrame* noia_frame_get_first(NoiaFrame* self);

/// Return last (most recently used) subframe.
NoiaFrame* noia_frame_get_last(NoiaFrame* self);

//------------------------------------------------------------------------------

/// Initialize iterator.
void noia_frame_start_iteration(NoiaFrameIterator* iter,
                                NoiaFrame* frame,
                                NoiaDirection direction);

/// Iterate through Frames.
void noia_frame_iterate(NoiaFrameIterator* iter);

//------------------------------------------------------------------------------

/// Print frame tree to log file.
void noia_frame_log(NoiaFrame* self, NoiaPrintFunc print, NoiaFrame* selection);

//------------------------------------------------------------------------------

#endif // NOIA_EXHIBITOR_FRAME_H

