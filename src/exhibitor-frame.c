// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-frame-internal.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_new(void)
{
    NoiaFrame* self = malloc(sizeof(*self));

    noia_branch_initialize(&self->branch);
    self->type = NOIA_FRAME_TYPE_NONE;
    self->sid = scInvalidSurfaceId;
    self->area.pos.x = 0;
    self->area.pos.y = 0;
    self->area.size.width = 0;
    self->area.size.height = 0;
    self->title = NULL;
    return self;
}

//------------------------------------------------------------------------------

void noia_frame_free(NoiaFrame* self)
{
    NOIA_ENSURE(self, return);

    // Free recursively whole tree
    NoiaFrame* next = NULL;
    NoiaFrame* twig = noia_frame_get_first(self);
    while (twig) {
        next = noia_frame_get_next(twig);
        noia_frame_free(twig);
        twig = next;
    }

    // Free frame
    noia_branch_finalize(&self->branch, NULL);
    if (self->title) {
        free(self->title);
    }
    memset(self, 0, sizeof(*self));
    free(self);
}

//------------------------------------------------------------------------------

void noia_frame_configure(NoiaFrame* self,
                          NoiaCoordinator* coordinator,
                          NoiaFrameType type,
                          NoiaSurfaceId sid,
                          NoiaArea area,
                          const char* title)
{
    NOIA_ENSURE(self, return);

    self->type = type;
    self->area = area;
    self->title = (title ? strdup(title) : NULL);
    noia_frame_set_surface(self, coordinator, sid);
}

//------------------------------------------------------------------------------

/// Return list of frames suitable for drawing.
/// @see noia_display_redraw_all
void noia_frame_to_array(NoiaFrame* self,
                         NoiaPool* surfaces,
                         NoiaCoordinator* coordinator)
{
    NOIA_ENSURE(self, return);

    NOIA_FOR_EACH_SUBFRAME_REVERSE (self, frame) {
        if (frame->sid != scInvalidSurfaceId) {
            noia_surface_to_array
                           (coordinator, frame->sid, frame->area.pos, surfaces);
        } else {
            noia_frame_to_array(frame, surfaces, coordinator);
        }
    }
}

//------------------------------------------------------------------------------

bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type)
{
    NOIA_ENSURE(self, return false);
    return (bool) (self->type & type);
}

//------------------------------------------------------------------------------

const char* noia_frame_get_title(NoiaFrame* self)
{
    NOIA_ENSURE(self, return "");
    return self->title;
}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_frame_get_sid(NoiaFrame* self)
{
    NOIA_ENSURE(self, return scInvalidSurfaceId);
    return self->sid;
}

//------------------------------------------------------------------------------

NoiaArea noia_frame_get_area(NoiaFrame* self)
{
    NoiaArea invalid_area = {{0, 0}, {0, 0}};
    NOIA_ENSURE(self, return invalid_area);
    return self->area;
}

//------------------------------------------------------------------------------

/// Swap surfaces between two frames and inform the surfaces if their sizes
/// changed.
NoiaResult noia_frame_swap(NoiaFrame* self,
                           NoiaFrame* frame,
                           NoiaCoordinator* coordinator)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(frame, return NOIA_RESULT_INCORRECT_ARGUMENT);

    NoiaSurfaceId sid = self->sid;
    self->sid = frame->sid;
    frame->sid = sid;

    if ((self->area.size.width  != frame->area.size.width)
    or  (self->area.size.height != frame->area.size.height)) {
        /// @todo Use set_desired_size
        noia_frame_set_size(self,  coordinator, self->area.size);
        noia_frame_set_size(frame, coordinator, frame->area.size);
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_frame_resize(NoiaFrame* self,
                       NoiaCoordinator* coordinator,
                       NoiaDirection border,
                       int magnitude)
{
    NOIA_ENSURE(self, return);

    // Find adequate frame for resize
    NoiaFrameType type = NOIA_FRAME_TYPE_NONE;
    if ((border == NOIA_DIRECTION_N) or (border == NOIA_DIRECTION_S)) {
        type = NOIA_FRAME_TYPE_VERTICAL;
    } else if ((border == NOIA_DIRECTION_E) or (border == NOIA_DIRECTION_W)) {
        type = NOIA_FRAME_TYPE_HORIZONTAL;
    }
    NoiaFrame* resizee = noia_frame_find_trunk_with_type(self, type);

    // If nothing found, don't worry, that happens
    NoiaFrame* trunk = noia_frame_get_trunk(resizee);
    if ((not resizee) or (not trunk)) {
        return;
    }

    // Resize using proper algorithm
    if (noia_frame_has_type(trunk, type)) {
        noia_frame_resize_anchored(resizee, coordinator, border, magnitude);
    } else if (not noia_frame_has_type(trunk, NOIA_FRAME_TYPE_SPECIAL)) {
        noia_frame_resize_floating(trunk, coordinator, border, magnitude);
    } else {
        // Special frames can't be resized anyway...
    }
}

//------------------------------------------------------------------------------

/// Move frame together with its contents.
/// The frame must be `floating` and not `fixed`.
/// @param direction - describes direction toward which frame should be moved.
/// @param magnituge - tells by how many pixels frame should be moved.
void noia_frame_move(NoiaFrame* self,
                     NoiaDirection direction,
                     int magnitude)
{
    NOIA_ENSURE(self, return);

    if ((not (self->type & NOIA_FRAME_TYPE_FLOATING))
     or (self->type & NOIA_FRAME_TYPE_FIXED)) {
        return;
    }

    NoiaPosition vector = {0, 0};
    if (direction == NOIA_DIRECTION_S) {
        vector.y += magnitude;
    }
    if (direction == NOIA_DIRECTION_N) {
        vector.y -= magnitude;
    }
    if (direction == NOIA_DIRECTION_E) {
        vector.x += magnitude;
    }
    if (direction == NOIA_DIRECTION_W) {
        vector.x -= magnitude;
    }

    noia_frame_move_with_contents(self, vector);
}

//------------------------------------------------------------------------------

/// Change direction (horizontal, vertical, stacked...) of the frame.
NoiaResult noia_frame_change_type(NoiaFrame* self,
                                  NoiaCoordinator* coordinator,
                                  NoiaFrameType type)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);

    self->type &= (~NOIA_FRAME_TYPE_DIRECTED);
    self->type |= type;

    noia_frame_relax(self, coordinator);

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Pop the surface `pop` inside surface `self`.
/// After calling this function `pop` will be most recently used frame inside
/// `self`.
void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(pop, return);

    // If there's nothing above or we reached `self` we can finish
    NoiaFrame* trunk = noia_frame_get_trunk(pop);
    if((trunk == NULL) or (self == pop)) {
        return;
    }

    // If it is `stacked` frame we have to pop it also spatially
    if (noia_frame_has_type(trunk, NOIA_FRAME_TYPE_STACKED)) {
        noia_branch_remove(&pop->branch);
        noia_branch_prepend(&trunk->branch, &pop->branch);
    }

    // Pop in temporal order
    noia_branch_pop(&pop->branch);

    // Do the same recursively on trunk
    noia_frame_pop_recursively(self, trunk);
}

//------------------------------------------------------------------------------

/// Remove given frame from its trunk and relax the trunk.
NoiaResult noia_frame_remove_self(NoiaFrame* self, NoiaCoordinator* coordinator)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);

    NoiaFrame* trunk = noia_frame_get_trunk(self);
    NOIA_ENSURE(trunk, return NOIA_RESULT_INCORRECT_ARGUMENT);

    noia_branch_remove(&self->branch);
    noia_frame_relax(trunk, coordinator);
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_settle(NoiaFrame* self,
                             NoiaFrame* target,
                             NoiaCoordinator* coordinator)
{
    noia_frame_relax(noia_frame_append(target, self), coordinator);
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_resettle(NoiaFrame* self,
                               NoiaFrame* target,
                               NoiaCoordinator* coordinator)
{
    NoiaResult result = noia_frame_remove_self(self, coordinator);
    if (result == NOIA_RESULT_SUCCESS) {
        noia_frame_settle(self, target, coordinator);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_ramify(NoiaFrame* self,
                             NoiaFrameType type,
                             NoiaCoordinator* coordinator)
{
    NoiaFrame* distancer = noia_frame_new();
    noia_branch_insert_before(&self->branch, &distancer->branch);
    noia_branch_remove(&self->branch);
    noia_branch_prepend(&distancer->branch, &self->branch);
    noia_frame_configure(distancer, coordinator, type, scInvalidSurfaceId,
                         noia_frame_get_area(self), NULL);
    return distancer;
}

//------------------------------------------------------------------------------

void noia_frame_deramify(NoiaFrame* self)
{
    unsigned len = noia_branch_get_number_of_twigs(&self->branch);

    if (len == 1) {
        NoiaFrame* first = noia_frame_get_first(self);
        len = noia_branch_get_number_of_twigs(&first->branch);
        if (len == 1) {
            NoiaFrame* second = noia_frame_get_first(first);
            noia_branch_remove(&second->branch);
            noia_branch_remove(&first->branch);
            noia_branch_prepend(&self->branch, &second->branch);
            noia_frame_free(first);
        } else if (len == 0) {
            self->type = NOIA_FRAME_TYPE_LEAF;
            self->sid = noia_frame_get_sid(first),
            noia_branch_remove(&first->branch);
            noia_frame_free(first);
        }
    }
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_jump(NoiaFrame* self,
                           NoiaFramePosition position,
                           NoiaFrame* target,
                           NoiaCoordinator* coordinator NOIA_UNUSED)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(target, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(target != self, return NOIA_RESULT_INCORRECT_ARGUMENT)

    NoiaResult result = noia_frame_remove_self(self, coordinator);
    if (result == NOIA_RESULT_SUCCESS) {
        result = noia_frame_jumpin(self, position, target, coordinator);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_jumpin(NoiaFrame* self,
                             NoiaFramePosition position,
                             NoiaFrame* target,
                             NoiaCoordinator* coordinator)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT)
    NOIA_ENSURE(target, return NOIA_RESULT_INCORRECT_ARGUMENT)
    NOIA_ENSURE(target != self, return NOIA_RESULT_INCORRECT_ARGUMENT)

    NoiaFrame* target_trunk = noia_frame_get_trunk(target);

    if (position == NOIA_FRAME_POSITION_BEFORE) {
        noia_branch_insert_before(&target->branch, &self->branch);
        noia_frame_relax(target_trunk, coordinator);
    } else if (position == NOIA_FRAME_POSITION_AFTER) {
        noia_branch_insert_after(&target->branch, &self->branch);
        noia_frame_relax(target_trunk, coordinator);
    } else if (position == NOIA_FRAME_POSITION_ON) {
        NoiaFrame* new_target = target;
        if (noia_branch_get_number_of_twigs(&target_trunk->branch) == 1) {
            new_target = target_trunk;
        } else if (noia_frame_has_type(target, NOIA_FRAME_TYPE_LEAF)) {
            new_target =
                noia_frame_ramify(target, NOIA_FRAME_TYPE_STACKED, coordinator);
        }
        noia_frame_settle(self, new_target, coordinator);
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid)
{
    return (NoiaFrame*) noia_branch_find(&self->branch, (void*) sid,
                                    (NoiaBranchCompare) noia_frame_compare_sid);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_pointed(NoiaFrame* self, NoiaPosition point)
{
    NOIA_ENSURE(self, return NULL);

    NoiaFrame* frame = NULL;
    point = noia_position_cast(point, self->area);

    if (noia_frame_has_type(self, NOIA_FRAME_TYPE_HORIZONTAL
                                | NOIA_FRAME_TYPE_VERTICAL)) {
        frame = self;
        NOIA_FOR_EACH_SUBFRAME (self, f) {
            if (noia_position_is_inside(point, f->area)) {
                frame = f;
                break;
            }
        }
    } else if (noia_frame_has_type(self, NOIA_FRAME_TYPE_LEAF)) {
        frame = self;
    } else {
        frame = noia_frame_get_recent(self);
    }

    return ((frame == self) or noia_frame_has_type(frame, NOIA_FRAME_TYPE_LEAF))
         ? frame : noia_frame_find_pointed(frame, point);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_contiguous(NoiaFrame* self,
                                      NoiaDirection direction,
                                      unsigned distance)
{
    NoiaFrame* trunk = noia_frame_get_trunk(self);
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(trunk, return NULL);

    // If distance is zero, this is the last step of recurrence.
    if (distance == 0) {
        return self;
    }

    // Find new frame which is farther.
    NoiaFrame* frame = NULL;
    int new_distance = distance;

    if (noia_frame_has_type(trunk, NOIA_FRAME_TYPE_VERTICAL)) {
        if (direction == NOIA_DIRECTION_N) {
            frame = noia_frame_get_previous(self);
        } else if (direction == NOIA_DIRECTION_S) {
            frame = noia_frame_get_next(self);
        }
    } else if (noia_frame_has_type(trunk, NOIA_FRAME_TYPE_HORIZONTAL)) {
        if (direction == NOIA_DIRECTION_W) {
            frame = noia_frame_get_previous(self);
        } else if (direction == NOIA_DIRECTION_E) {
            frame = noia_frame_get_next(self);
        }
    }

    // If there is nothing farther go higher. If it is, decrease distance.
    if (frame or (direction == NOIA_DIRECTION_TRUNK)) {
        new_distance = distance - 1;
    }
    if (not frame) {
        frame = trunk;
    }

    // Next recurrence step if possible.
    if (frame and noia_frame_get_trunk(frame)
    and (not noia_frame_has_type(frame, NOIA_FRAME_TYPE_SPECIAL))) {
        return noia_frame_find_contiguous(frame, direction, new_distance);
    } else {
        return NULL;
    }
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_adjacent(NoiaFrame* self,
                                    NoiaDirection direction,
                                    unsigned distance)
{
    NOIA_ENSURE(self, return NULL);

    // If distance is zero, this is the last step of recurrence.
    if (distance == 0) {
        return self;
    }

    // Calculate reference position
    NoiaPosition point = {
            (self->area.pos.x + self->area.size.width) / 2,
            (self->area.pos.y + self->area.size.height) / 2
        };

    // Search for the frame
    NoiaFrame* frame = self;
    for (unsigned i = 0; i < distance; ++i) {
        frame = noia_frame_find_contiguous(frame, direction, 1);
        if (not frame) break;
        frame = noia_frame_find_pointed(frame, point);
        if (not frame) break;
    }
    return frame;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_top(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);

    NoiaFrame* frame = self;
    NoiaFrame* trunk = noia_frame_get_trunk(self);

    if ((not trunk) and noia_frame_has_type(self, NOIA_FRAME_TYPE_SPECIAL)) {
        frame = NULL;
    }

    while (trunk and (not noia_frame_has_type(trunk, NOIA_FRAME_TYPE_SPECIAL))){
        frame = trunk;
        trunk = noia_frame_get_trunk(trunk);
    }
    return frame;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_buildable(NoiaFrame* self)
{
    return (self->sid == scInvalidSurfaceId)
         ? self : noia_frame_get_trunk(self);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_trunk(NoiaFrame* self)
{
    return (NoiaFrame*) self->branch.trunk;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_first(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_first(&self->branch);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_last(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_last(&self->branch);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_recent(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_recent(&self->branch);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_former(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_former(&self->branch);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_next(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_next(&self->branch);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_previous(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_previous(&self->branch);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_later(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_later(&self->branch);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_earlier(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NULL);
    return (NoiaFrame*) noia_branch_get_earlier(&self->branch);
}

//------------------------------------------------------------------------------

void noia_frame_log(NoiaFrame* self, NoiaPrintFunc print, NoiaFrame* selection)
{
    noia_frame_log_internal(self, print, selection, 0);
}

//------------------------------------------------------------------------------

