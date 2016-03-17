// file: exhibitor-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

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
    if (!self) {
        return;
    }

    noia_branch_free(self, (NoiaFreeFunc) noia_frame_params_free);
}

//------------------------------------------------------------------------------

void noia_frame_configure(NoiaFrame* self,
                          NoiaFrameType type,
                          NoiaSurfaceId sid,
                          NoiaPosition pos,
                          NoiaSize size,
                          const char* title)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    params->type = type;
    params->area.pos = pos;
    params->area.size = size;
    params->title = (title ? strdup(title) : NULL);
    noia_frame_set_surface(self, sid);
}

//------------------------------------------------------------------------------

void noia_frame_configure_as_workspace(NoiaFrame* self,
                                       NoiaSize size,
                                       const char* title)
{
    noia_frame_configure(self, NOIA_FRAME_TYPE_WORKSPACE,
                         scInvalidSurfaceId, (NoiaPosition) {0,0}, size, title);
}

//------------------------------------------------------------------------------

void noia_frame_to_array(NoiaFrame* self, NoiaPool* surfaces)
{
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

NoiaResult noia_frame_swap(NoiaFrame* self, NoiaFrame* frame)
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
        noia_frame_set_size(self,  param1->area.size);
        noia_frame_set_size(frame, param2->area.size);
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_frame_resize(NoiaFrame* self,
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
        noia_frame_resize_anchored(resizee, border, magnitude);
    } else if (not noia_frame_has_type(trunk, NOIA_FRAME_TYPE_SPECIAL)) {
        noia_frame_resize_floating(trunk, border, magnitude);
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

NoiaResult noia_frame_change_type(NoiaFrame* self, NoiaFrameType type)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);

    int len = noia_chain_len(self->twigs);
    NOIA_ENSURE(len > 0, return NOIA_RESULT_INCORRECT_ARGUMENT);

    // Decide how to resize and move twigs
    NoiaFrameParams* params = noia_frame_get_params(self);
    NoiaSize size = {0, 0};
    NoiaSize increment = {0, 0};
    if (type == NOIA_FRAME_TYPE_STACKED) {
        size = params->area.size;
    } else if (type == NOIA_FRAME_TYPE_VERTICAL) {
        size.width = params->area.size.width;
        size.height = increment.height = params->area.size.height / len;
    } else if (type == NOIA_FRAME_TYPE_HORIZONTAL) {
        size.height = params->area.size.height;
        size.width = increment.width = params->area.size.width / len;
    } else {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    // Resize and reposition all twigs
    NoiaPosition pos = params->area.pos;
    FOR_EACH_TWIG (self, twig) {
        noia_frame_set_size(twig, size);
        noia_frame_set_position(twig, pos);
        pos.x += increment.width;
        pos.y += increment.height;
    }

    // Change type
    params->type &= (~NOIA_FRAME_TYPE_DIRECTED);
    params->type |= type;

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

NoiaResult noia_frame_remove_self(NoiaFrame* self)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(self->trunk, return NOIA_RESULT_INCORRECT_ARGUMENT);

    return noia_branch_remove(self->trunk, self);
}

//------------------------------------------------------------------------------

/// @todo Fix setting size and position.
NoiaResult noia_frame_jump(NoiaFrame* self, NoiaFrame* target)
{
    return noia_frame_resettle(self, target);
}

//------------------------------------------------------------------------------

/// @todo
NoiaResult noia_frame_jumpin(NoiaFrame* self, NoiaFrame* target)
{
    noia_frame_append(self, target);
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

