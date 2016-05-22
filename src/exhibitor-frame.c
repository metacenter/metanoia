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
    NoiaFrame* self = noia_branch_new();

    self->base.data = malloc(sizeof(NoiaFrameParams));
    NOIA_ENSURE(self->base.data, abort());

    NoiaFrameParams* params = self->base.data;
    memset(params, 0, sizeof(NoiaFrameParams));
    params->type = NOIA_FRAME_TYPE_NONE;
    params->sid = scInvalidSurfaceId;
    params->area.pos.x = 0;
    params->area.pos.y = 0;
    params->area.size.width = 0;
    params->area.size.height = 0;
    return self;
}

//------------------------------------------------------------------------------

void noia_frame_free(NoiaFrame* self)
{
    NOIA_ENSURE(self, return);
    noia_branch_free(self, (NoiaFreeFunc) noia_frame_params_free);
}

//------------------------------------------------------------------------------

void noia_frame_configure(NoiaFrame* self,
                          NoiaCoordinator* coordinator,
                          NoiaFrameType type,
                          NoiaSurfaceId sid,
                          NoiaArea area,
                          const char* title)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    params->type = type;
    params->area = area;
    params->title = (title ? strdup(title) : NULL);
    noia_frame_set_surface(self, coordinator, sid);
}

//------------------------------------------------------------------------------

void noia_frame_to_array(NoiaFrame* self, NoiaPool* surfaces)
{
    NOIA_ENSURE(self, return);

    FOR_EACH_TWIG(self, twig) {
        NoiaFrameParams* params = noia_frame_get_params(twig);
        if (params->sid != scInvalidSurfaceId) {
            NoiaSurfaceContext* context = noia_pool_add(surfaces);
            context->sid = params->sid;
            context->pos = params->area.pos;
        } else {
            noia_frame_to_array(twig, surfaces);
        }
    }
}

//------------------------------------------------------------------------------

bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type)
{
    NOIA_ENSURE(self, return false);
    return !!(noia_frame_get_params(self)->type & type);
}

//------------------------------------------------------------------------------

const char* noia_frame_get_title(NoiaFrame* self)
{
    NOIA_ENSURE(self, return "");
    return noia_frame_get_params(self)->title;
}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_frame_get_sid(NoiaFrame* self)
{
    NOIA_ENSURE(self, return scInvalidSurfaceId);
    return noia_frame_get_params(self)->sid;
}

//------------------------------------------------------------------------------

NoiaArea noia_frame_get_area(NoiaFrame* self)
{
    NoiaArea invalid_area = {{0, 0}, {0, 0}};
    NOIA_ENSURE(self, return invalid_area);
    return noia_frame_get_params(self)->area;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_swap(NoiaFrame* self,
                           NoiaFrame* frame,
                           NoiaCoordinator* coordinator)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(frame, return NOIA_RESULT_INCORRECT_ARGUMENT);

    NoiaFrameParams* param1 = noia_frame_get_params(self);
    NoiaFrameParams* param2 = noia_frame_get_params(frame);

    NoiaSurfaceId sid = param1->sid;
    param1->sid = param2->sid;
    param2->sid = sid;

    if ((param1->area.size.width  != param2->area.size.width)
    or  (param1->area.size.height != param2->area.size.height)) {
        /// @todo Use set_desired_size
        noia_frame_set_size(self,  coordinator, param1->area.size);
        noia_frame_set_size(frame, coordinator, param2->area.size);
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_frame_resize(NoiaFrame* self,
                       NoiaCoordinator* coordinator,
                       NoiaArgmand border,
                       int magnitude)
{
    NOIA_ENSURE(self, return);

    // Find adequate frame for resize
    NoiaFrameType type = NOIA_FRAME_TYPE_NONE;
    if ((border == NOIA_ARGMAND_N) or (border == NOIA_ARGMAND_S)) {
        type = NOIA_FRAME_TYPE_VERTICAL;
    } else if ((border == NOIA_ARGMAND_E) or (border == NOIA_ARGMAND_W)) {
        type = NOIA_FRAME_TYPE_HORIZONTAL;
    }
    NoiaFrame* resizee = noia_frame_find_trunk_with_type(self, type);

    // If nothing found, don't worry, that happens
    if ((not resizee) or (not resizee->trunk)) {
        return;
    }

    // Resize using proper algorithm
    NoiaFrame* trunk = resizee->trunk;
    if (noia_frame_has_type(trunk, type)) {
        noia_frame_resize_anchored(resizee, coordinator, border, magnitude);
    } else if (not noia_frame_has_type(trunk, NOIA_FRAME_TYPE_SPECIAL)) {
        noia_frame_resize_floating(trunk, coordinator, border, magnitude);
    } else {
        // Special frames can't be resized anyway...
    }
}

//------------------------------------------------------------------------------

void noia_frame_move(NoiaFrame* self,
                     NoiaArgmand direction,
                     int magnitude)
{
    NOIA_ENSURE(self, return);

    NoiaFrameParams* params = noia_frame_get_params(self);
    if ((not (params->type & NOIA_FRAME_TYPE_FLOATING))
     or (params->type & NOIA_FRAME_TYPE_FIXED)) {
        return;
    }

    NoiaPosition vector = {0, 0};
    if (direction == NOIA_ARGMAND_S) {
        vector.y += magnitude;
    }
    if (direction == NOIA_ARGMAND_N) {
        vector.y -= magnitude;
    }
    if (direction == NOIA_ARGMAND_E) {
        vector.x += magnitude;
    }
    if (direction == NOIA_ARGMAND_W) {
        vector.x -= magnitude;
    }

    noia_frame_move_with_contents(self, vector);
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_change_type(NoiaFrame* self,
                                  NoiaCoordinator* coordinator,
                                  NoiaFrameType type)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);

    NoiaFrameParams* params = noia_frame_get_params(self);
    params->type &= (~NOIA_FRAME_TYPE_DIRECTED);
    params->type |= type;

    noia_frame_relax(self, coordinator);

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(pop, return);

    if((not pop->trunk) or (self == pop)) {
        return;
    }

    if (noia_frame_has_type(pop->trunk, NOIA_FRAME_TYPE_STACKED)) {
        NoiaBranch* trunk = pop->trunk;
        noia_branch_remove(trunk, pop);
        noia_branch_append(trunk, pop);
    }

    noia_frame_pop_recursively(self, pop->trunk);
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_remove_self(NoiaFrame* self, NoiaCoordinator* coordinator)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(self->trunk, return NOIA_RESULT_INCORRECT_ARGUMENT);

    NoiaFrame* trunk = self->trunk;
    NoiaResult result = noia_branch_remove(self->trunk, self);
    noia_frame_relax(trunk, coordinator);
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_jump(NoiaFrame* self,
                           NoiaFrame* target,
                           NoiaCoordinator* coordinator NOIA_UNUSED)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(self->trunk, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(target, return NOIA_RESULT_INCORRECT_ARGUMENT);

    NoiaResult result = NOIA_RESULT_SUCCESS;
    /// @todo To be implemented.
    return result;
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
    NoiaFrame* source = self->trunk;
    NoiaResult result = noia_frame_remove_self(self, coordinator);
    if (result == NOIA_RESULT_SUCCESS) {
        noia_frame_prepend(target, self);
        noia_frame_relax(source, coordinator);
        noia_frame_relax(target, coordinator);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_ramify(NoiaFrame* self,
                             NoiaFrameType type,
                             NoiaCoordinator* coordinator)
{
    NoiaFrame* distancer = noia_frame_new();
    noia_branch_insert_before(self, distancer);
    noia_branch_remove(self->trunk, self);
    noia_branch_prepend(distancer, self);
    noia_frame_configure(distancer, coordinator, type, scInvalidSurfaceId,
                         noia_frame_get_area(self), NULL);
    return distancer;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_jumpin(NoiaFrame* self,
                             NoiaFramePosition position,
                             NoiaFrame* target,
                             NoiaCoordinator* coordinator)
{
    if (position == NOIA_FRAME_POSITION_BEFORE) {
        noia_branch_insert_before(target, self);
        noia_frame_relax(target->trunk, coordinator);
    } else if (position == NOIA_FRAME_POSITION_AFTER) {
        noia_branch_insert_after(target, self);
        noia_frame_relax(target->trunk, coordinator);
    } else if (position == NOIA_FRAME_POSITION_ON) {
        NoiaFrame* distancer =
                noia_frame_ramify(target, NOIA_FRAME_TYPE_STACKED, coordinator);
        noia_frame_settle(self, distancer, coordinator);
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid)
{
    return noia_branch_find(self, (void*) sid,
                           (NoiaBranchCompare) noia_frame_params_compare_sid);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_pointed(NoiaFrame* self, NoiaPosition point)
{
    NOIA_ENSURE(self, return NULL);

    NoiaFrame* frame = NULL;
    NoiaFrameParams* params = noia_frame_get_params(self);
    point = noia_position_cast(point, params->area);

    if (noia_frame_has_type(self, NOIA_FRAME_TYPE_HORIZONTAL
                                | NOIA_FRAME_TYPE_VERTICAL)) {
        frame = self;
        FOR_EACH_TWIG(self, f) {
            params = noia_frame_get_params(f);
            if (noia_position_is_inside(point, params->area)) {
                frame = f;
                break;
            }
        }
    } else if (noia_frame_has_type(self, NOIA_FRAME_TYPE_LEAF)) {
        frame = self;
    } else if (self->twigs) {
        frame = (NoiaFrame*) self->twigs->first;
    }

    return ((frame == self) or noia_frame_has_type(frame, NOIA_FRAME_TYPE_LEAF))
         ? frame : noia_frame_find_pointed(frame, point);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_contiguous(NoiaFrame* self,
                                      NoiaArgmand direction,
                                      unsigned distance)
{
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(self->trunk, return NULL);
    NOIA_ENSURE(noia_argmand_is_directed(direction), return NULL);

    // If distance is zero, this is the last step of recurrence.
    if (distance == 0) {
        return self;
    }

    // Find new frame which is farther.
    NoiaFrame* frame = NULL;
    int new_distance = distance;

    if (noia_frame_has_type(self->trunk, NOIA_FRAME_TYPE_VERTICAL)) {
        if (direction == NOIA_ARGMAND_N) {
            frame = noia_branch_get_previous(self);
        } else if (direction == NOIA_ARGMAND_S) {
            frame = noia_branch_get_next(self);
        }
    } else if (noia_frame_has_type(self->trunk, NOIA_FRAME_TYPE_HORIZONTAL)) {
        if (direction == NOIA_ARGMAND_W) {
            frame = noia_branch_get_previous(self);
        } else if (direction == NOIA_ARGMAND_E) {
            frame = noia_branch_get_next(self);
        }
    }

    // If there is nothing farther go higher. If it is, decrease distance.
    if (frame or (direction == NOIA_ARGMAND_TRUNK)) {
        new_distance = distance - 1;
    }
    if (not frame) {
        frame = self->trunk;
    }

    // Next recurrence step if possible.
    if (frame and frame->trunk
    and (not noia_frame_has_type(frame, NOIA_FRAME_TYPE_SPECIAL))) {
        return noia_frame_find_contiguous(frame, direction, new_distance);
    } else {
        return NULL;
    }
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_adjacent(NoiaFrame* self,
                                    NoiaArgmand direction,
                                    unsigned distance)
{
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(noia_argmand_is_directed(direction), return NULL);

    // If distance is zero, this is the last step of recurrence.
    if (distance == 0) {
        return self;
    }

    // Calculate reference position
    NoiaFrameParams* params = noia_frame_get_params(self);
    NoiaPosition point = {
            (params->area.pos.x + params->area.size.width) / 2,
            (params->area.pos.y + params->area.size.height) / 2
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
    while (frame and (not noia_frame_has_type(frame, NOIA_FRAME_TYPE_SPECIAL))){
        frame = frame->trunk;
    }
    return frame;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_buildable(NoiaFrame* self)
{
    return (noia_frame_get_params(self)->sid == scInvalidSurfaceId)
         ? self : self->trunk;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_parent(NoiaFrame* self)
{
    return (NoiaFrame*) self->trunk;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_first(NoiaFrame* self)
{
    return (NoiaFrame*) self->twigs->first;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_get_last(NoiaFrame* self)
{
    return (NoiaFrame*) self->twigs->last;
}

//------------------------------------------------------------------------------

void noia_frame_start_iteration(NoiaFrameIterator* iter,
                                NoiaFrame* frame,
                                NoiaArgmand direction)
{
    NOIA_ENSURE(iter, return);

    iter->num = 0;
    iter->direction = direction;
    iter->position = NOIA_FRAME_POSITION_ON;
    iter->frame = noia_frame_find_trunk_with_type
                                            (frame, NOIA_FRAME_TYPE_VERTICAL
                                                  | NOIA_FRAME_TYPE_HORIZONTAL);
}

//------------------------------------------------------------------------------

/// @todo Add support for NOIA_ARGMAND_END which means to incorporate current
///       frame to the one below in stacked frame.
void noia_frame_iterate(NoiaFrameIterator* iter)
{
    NOIA_ENSURE(iter, return);
    NOIA_ENSURE(iter->frame, return);

    NoiaFrame* next_frame = iter->frame;

    NOIA_BLOCK {
        NoiaFrame* simple_next = NULL;
        if ((iter->direction == NOIA_ARGMAND_N)
        or  (iter->direction == NOIA_ARGMAND_W)) {
            simple_next = (NoiaFrame*) iter->frame->base.prev;
            if (iter->position == NOIA_FRAME_POSITION_ON) {
                iter->position = NOIA_FRAME_POSITION_BEFORE;
                break;
            } else if (iter->position == NOIA_FRAME_POSITION_AFTER) {
                iter->position = NOIA_FRAME_POSITION_ON;
                break;
            } else {
                iter->position = NOIA_FRAME_POSITION_AFTER;
            }
        } else if ((iter->direction == NOIA_ARGMAND_S)
               or  (iter->direction == NOIA_ARGMAND_E)) {
            simple_next = (NoiaFrame*) iter->frame->base.next;
            if (iter->position == NOIA_FRAME_POSITION_ON) {
                iter->position = NOIA_FRAME_POSITION_AFTER;
                break;
            } else if (iter->position == NOIA_FRAME_POSITION_BEFORE) {
                iter->position = NOIA_FRAME_POSITION_ON;
                break;
            } else {
                iter->position = NOIA_FRAME_POSITION_BEFORE;
            }
        }

        next_frame = noia_frame_find_adjacent(iter->frame, iter->direction, 1);
        if (not next_frame) {
            break;
        }
        next_frame = noia_frame_find_trunk_with_type
             (next_frame, NOIA_FRAME_TYPE_VERTICAL | NOIA_FRAME_TYPE_HORIZONTAL);
        if (not next_frame) {
            break;
        }

        if (simple_next == next_frame) {
            iter->position = NOIA_FRAME_POSITION_ON;
        }
    }

    iter->num += 1;
    iter->frame = next_frame;
}

//------------------------------------------------------------------------------

void noia_frame_log(NoiaFrame* self, NoiaPrintFunc print, NoiaFrame* selection)
{
    noia_frame_log_internal(self, print, selection, 0);
}

//------------------------------------------------------------------------------

