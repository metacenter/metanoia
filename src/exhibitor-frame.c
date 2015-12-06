// file: exhibitor-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "surface-manager.h"

/// @todo: remove event signals from exhibitor frame
#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------
// PRIVATE

/// Get the parameters - convenience function.
static inline NoiaFrameParams* noia_frame_get_params(NoiaFrame* self)
{
    return self->base.data;
}

//------------------------------------------------------------------------------

/// Compare function - check if given params have give surface ID.
int noia_frame_params_compare_sid(NoiaFrameParams* params, NoiaSurfaceId sid)
{
    NoiaSurfaceId frame_sid = scInvalidSurfaceId;
    if (params) {
        frame_sid = params->sid;
    }

    if (frame_sid < sid) {
        return -1;
    } else if (frame_sid > sid) {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------

/// Parameters destructor.
void noia_frame_params_free(NoiaFrameParams* params)
{
    if (!params) {
        return;
    }

    if (params->title) {
        free(params->title);
    }

    memset(params, 0, sizeof(NoiaFrameParams));
    free(params);
}

//------------------------------------------------------------------------------

/// Set surface ID.
void noia_frame_set_surface(NoiaFrame* self, NoiaSurfaceId sid)
{
    noia_frame_get_params(self)->sid = sid;

    // Resize to requested size
    if (sid != scInvalidSurfaceId) {
        NoiaSize size = {INT_MIN, INT_MIN};
        noia_surface_set_desired_size(sid, size);
    }
}

//------------------------------------------------------------------------------

/// Set frame size.
void noia_frame_set_size(NoiaFrame* self, NoiaSize size)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    params->area.size = size;
    noia_surface_set_desired_size(params->sid, size);
}

//------------------------------------------------------------------------------

/// Move the frame with all twigs.
void noia_frame_move_with_contents(NoiaFrame* self, NoiaPosition vector)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    // Update surfaces position
    params->area.pos.x += vector.x;
    params->area.pos.y += vector.y;

    // Move all subframes
    FOR_EACH_TWIG(self, branch) {
        noia_frame_move_with_contents(branch, vector);
    }
}

//------------------------------------------------------------------------------

/// Set frame position.
void noia_frame_set_position(NoiaFrame* self, NoiaPosition position)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    NoiaPosition vector;
    vector.x = position.x - params->area.pos.x;
    vector.y = position.y - params->area.pos.y;

    noia_frame_move_with_contents(self, vector);
}

//------------------------------------------------------------------------------

/// Resize the surface frame is holding.
void noia_frame_reconfigure(NoiaFrame* self,
                            NoiaArgmand direction,
                            int magnitude)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    if (direction == NOIA_ARGMAND_N || direction == NOIA_ARGMAND_S) {
        params->area.size.height += magnitude;
    }
    if (direction == NOIA_ARGMAND_E || direction == NOIA_ARGMAND_W) {
        params->area.size.width += magnitude;
    }

    noia_surface_set_desired_size(params->sid, params->area.size);

    /// @todo Implement noia_frame_reconfigure for twigs.
}

//------------------------------------------------------------------------------

/// Resize floating frame.
void noia_frame_resize_floating(NoiaFrame* self,
                                NoiaArgmand direction,
                                int magnitude)
{
    noia_frame_reconfigure(self, direction, magnitude);
    if (direction == NOIA_ARGMAND_W || direction == NOIA_ARGMAND_N) {
        noia_frame_move(self, direction, magnitude);
    }
}

//------------------------------------------------------------------------------

/// Resize anchored frame.
void noia_frame_resize_anchored(NoiaFrame* self,
                                NoiaArgmand direction,
                                int magnitude)
{
    if (direction == NOIA_ARGMAND_N || direction == NOIA_ARGMAND_W) {
        if (self->base.prev) {
            noia_frame_reconfigure(self, direction, magnitude);
            noia_frame_reconfigure((NoiaFrame*) self->base.prev,
                                   direction == NOIA_ARGMAND_N ?
                                   NOIA_ARGMAND_W : NOIA_ARGMAND_N,
                                   -magnitude);
        }
    }

    if (direction == NOIA_ARGMAND_S || direction == NOIA_ARGMAND_E) {
        if (self->base.next) {
            noia_frame_reconfigure(self, direction, magnitude);
            noia_frame_reconfigure((NoiaFrame*) self->base.next,
                                   direction == NOIA_ARGMAND_S ?
                                   NOIA_ARGMAND_E : NOIA_ARGMAND_N,
                                   -magnitude);
        }
    }
}

//------------------------------------------------------------------------------

/// Helper function for log printing.
/// @see noia_frame_log
static inline void noia_frame_log_internal(NoiaFrame* self,
                                           NoiaPrintFunc print,
                                           NoiaFrame* selection,
                                           unsigned level)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    for (unsigned i = 0; i < level; ++i) {
        print("    ");
    }

    print("NoiaFrame(type='0x%x', sid='%d', len='%d'"
          "x='%d', y='%d', w='%d', h='%d')%s\n",
          params->type, params->sid, noia_chain_len(self->twigs),
          params->area.pos.x, params->area.pos.y,
          params->area.size.width, params->area.size.height,
          (self == selection) ? " <== FOCUS" : "");

    FOR_EACH_TWIG(self, twig) {
        noia_frame_log_internal(twig, print, selection, level+1);
    }
}

//------------------------------------------------------------------------------
// PUBLIC

bool noia_frame_parameters_are_equivalent(NoiaFrameParams* p1,
                                          NoiaFrameParams* p2)
{
    if (!p1 || !p2) {
        return false;
    }

    return p1->type             == p2->type
        && p1->sid              == p2->sid
        && p1->area.pos.x       == p2->area.pos.x
        && p1->area.pos.y       == p2->area.pos.y
        && p1->area.size.width  == p2->area.size.width
        && p1->area.size.height == p2->area.size.height;
}

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

NoiaFrame* noia_frame_create_child(NoiaFrame* self, NoiaFrameType type)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    if (!(params->type & NOIA_FRAME_TYPE_STACKED)) {
        return NULL;
    }

    NoiaFrame* frame = noia_frame_new();
    noia_frame_configure(frame, type, scInvalidSurfaceId,
                         params->area.pos, params->area.size);
    noia_frame_append(self, frame);

    return frame;
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
                          NoiaSize size)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    params->type = type;
    params->area.pos = pos;
    params->area.size = size;
    noia_frame_set_surface(self, sid);
}

//------------------------------------------------------------------------------

void noia_frame_configure_as_workspace(NoiaFrame* self,
                                       NoiaSize size)
{
    noia_frame_configure(self, NOIA_FRAME_TYPE_WORKSPACE,
                         scInvalidSurfaceId, (NoiaPosition) {0,0}, size);
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

void noia_frame_append(NoiaFrame* self, NoiaFrame* other)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(other, return);

    if (noia_frame_get_params(self)->sid == scInvalidSurfaceId) {
        noia_branch_append(self, other);
    } else {
        noia_branch_append(self->trunk, other);
    }
}

//------------------------------------------------------------------------------

void noia_frame_prepend(NoiaFrame* self, NoiaFrame* other)
{
    if (noia_frame_get_params(self)->sid == scInvalidSurfaceId) {
        noia_branch_prepend(self, other);
    } else {
        noia_branch_prepend(self->trunk, other);
    }
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
        noia_frame_set_size(self,  param1->area.size);
        noia_frame_set_size(frame, param2->area.size);
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_resettle(NoiaFrame* self, NoiaFrame* target)
{
    NoiaResult result = NOIA_RESULT_SUCCESS;
    NOIA_BLOCK {
        result = noia_frame_remove_self(self);
        NOIA_ASSERT_RESULT(result);

        noia_frame_prepend(target, self);
        noia_frame_set_size(self, noia_frame_get_params(target)->area.size);
    }
    return result;
}

//------------------------------------------------------------------------------

void noia_frame_resize(NoiaFrame* self,
                       NoiaArgmand direction,
                       int magnitude)
{
    NOIA_ENSURE(self, return);

    // Find adequate frame for resize
    NoiaFrameType type = NOIA_FRAME_TYPE_NONE;
    if (direction == NOIA_ARGMAND_N || direction == NOIA_ARGMAND_S) {
        type = NOIA_FRAME_TYPE_VERTICAL;
    } else if (direction == NOIA_ARGMAND_E || direction == NOIA_ARGMAND_W) {
        type = NOIA_FRAME_TYPE_HORIZONTAL;
    }
    NoiaFrame* resizee = noia_frame_find_trunk_with_type(self, type);

    // If nothing found, don't worry, that happens
    if (!resizee) {
        return;
    }

    // Resize using proper algorithm
    if (noia_frame_has_type(resizee->trunk, NOIA_FRAME_TYPE_FLOATING)) {
        noia_frame_resize_floating(resizee->trunk, direction, magnitude);
    } else if (!noia_frame_has_type(resizee->trunk, NOIA_FRAME_TYPE_SPECIAL)) {
        noia_frame_resize_anchored(resizee, direction, magnitude);
    } else {
        // Special frames can't be resized anyway...
    }
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

void noia_frame_move(NoiaFrame* self,
                     NoiaArgmand direction,
                     int magnitude)
{
    NOIA_ENSURE(self, return);

    NoiaFrameParams* params = noia_frame_get_params(self);
    if (!(params->type & NOIA_FRAME_TYPE_FLOATING)
      || (params->type & NOIA_FRAME_TYPE_FIXED)) {
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

void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop)
{
    if (!self || !pop || !pop->trunk || self == pop) {
        return;
    }

    if (noia_frame_get_params(pop->trunk)->type & NOIA_FRAME_TYPE_STACKED) {
        NoiaBranch* trunk = pop->trunk;
        noia_branch_remove(trunk, pop);
        noia_branch_append(trunk, pop);
    }

    noia_frame_pop_recursively(self, pop->trunk);
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_remove_self(NoiaFrame* self)
{
    if (!self || !self->trunk) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    return noia_branch_remove(self->trunk, self);
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
    while (frame && !noia_frame_has_type(frame, NOIA_FRAME_TYPE_SPECIAL)) {
        frame = frame->trunk;
    }
    return frame;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_trunk_with_type(NoiaFrame* frame, NoiaFrameType type)
{
    NOIA_ENSURE(frame, return NULL);
    NOIA_ENSURE(frame->trunk, return NULL);

    if ((noia_frame_get_params(frame->trunk)->type & NOIA_FRAME_TYPE_FLOATING)
    or  (noia_frame_get_params(frame->trunk)->type & NOIA_FRAME_TYPE_SPECIAL)
    or  (noia_frame_get_params(frame->trunk)->type & type)) {
        return frame;
    }
    return noia_frame_find_trunk_with_type(frame->trunk, type);
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

