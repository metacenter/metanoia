// file: exhibitor-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "utils-log.h"
#include "surface-manager.h"
#include "global-macros.h"

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
/// @todo Remove?
void noia_frame_set_size(NoiaFrame* self, NoiaSize size)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    params->size = size;
    noia_surface_set_desired_size(params->sid, size);
}

//------------------------------------------------------------------------------

/// Find first ancestor tha can be resize in given direction.
NoiaFrame* noia_frame_resize_find_helper(NoiaFrame* frame,
                                         NoiaFrameType type)
{
    if (noia_frame_get_params(frame)->type & NOIA_FRAME_TYPE_FLOATING
    ||  noia_frame_get_params(frame->trunk)->type & type) {
        return frame;
    }
    return noia_frame_resize_find_helper(frame->trunk, type);
}

//------------------------------------------------------------------------------

/// Resize the surface frame is holding.
void noia_frame_reconfigure(NoiaFrame* self,
                            NoiaArgmand direction,
                            int magnitude)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    if (direction == NOIA_ARGMAND_N || direction == NOIA_ARGMAND_S) {
        params->size.height += magnitude;
    }
    if (direction == NOIA_ARGMAND_E || direction == NOIA_ARGMAND_W) {
        params->size.width += magnitude;
    }

    noia_surface_set_desired_size(params->sid, params->size);

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

/// Move the frame with all children.
void noia_frame_move_with_contents(NoiaFrame* self,
                                   NoiaPosition vector)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    // Update surfaces position
    params->pos.x += vector.x;
    params->pos.y += vector.y;

    // Move all subframes
    FOR_EACH_TWIG(self, branch) {
        noia_frame_move_with_contents(branch, vector);
    }
}

//------------------------------------------------------------------------------

/// Helper function for log printing.
/// @see noia_frame_log
static inline void noia_frame_log_internal(NoiaFrame* self, unsigned level)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    if (level == 0) {
        noia_log_begin("FRAMES");
    }

    for (unsigned i = 0; i < level; ++i) {
        noia_log_print("    ");
    }

    noia_log_print("NoiaFrame(type='0x%x', sid='%d', len='%d')\n",
                   params->type, params->sid, noia_chain_len(self->twigs));

    FOR_EACH_TWIG(self, twig) {
        noia_frame_log_internal(twig, level+1);
    }

    if (level == 0) {
        noia_log_end();
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

    return p1->type        == p2->type
        && p1->sid         == p2->sid
        && p1->pos.x       == p2->pos.x
        && p1->pos.y       == p2->pos.y
        && p1->size.width  == p2->size.width
        && p1->size.height == p2->size.height;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_new()
{
    NoiaFrame* self = noia_branch_new();

    self->base.data = malloc(sizeof(NoiaFrameParams));
    assert(self->base.data);

    NoiaFrameParams* params = self->base.data;
    memset(params, 0, sizeof(NoiaFrameParams));
    params->type = NOIA_FRAME_TYPE_NONE;
    params->sid = scInvalidSurfaceId;
    params->pos.x = 0;
    params->pos.y = 0;
    params->size.width = 0;
    params->size.height = 0;
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
                         params->pos, params->size);
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
    params->pos = pos;
    params->size = size;
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
            context->pos = params->pos;
        } else {
            noia_frame_to_array(twig, surfaces);
        }
    }
}

//------------------------------------------------------------------------------

bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type)
{
    return !!(noia_frame_get_params(self)->type & type);
}

//------------------------------------------------------------------------------

void noia_frame_append(NoiaFrame* self, NoiaFrame* other)
{
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

NoiaFrame* noia_frame_get_top(NoiaFrame* self)
{
    assert(self);
    NoiaFrame* frame = self;
    while (frame && !noia_frame_has_type(frame, NOIA_FRAME_TYPE_SPECIAL)) {
        frame = frame->trunk;
    }
    return frame;
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_resettle(NoiaFrame* self, NoiaFrame* target)
{
    NoiaResult result = NOIA_RESULT_SUCCESS;
    NOIA_TRY {
        result = noia_frame_remove_self(self);
        NOIA_ASSERT_RESULT(result);

        noia_frame_prepend(target, self);
        noia_frame_set_size(self, noia_frame_get_params(target)->size);
    }
    return result;
}

//------------------------------------------------------------------------------

void noia_frame_resize(NoiaFrame* self,
                       NoiaArgmand direction,
                       int magnitude)
{
    if (!self) {
        return;
    }

    // Find adequate frame for resize
    NoiaFrameType type = NOIA_FRAME_TYPE_NONE;
    if (direction == NOIA_ARGMAND_N || direction == NOIA_ARGMAND_S) {
        type = NOIA_FRAME_TYPE_VERTICAL;
    } else if (direction == NOIA_ARGMAND_E || direction == NOIA_ARGMAND_W) {
        type = NOIA_FRAME_TYPE_HORIZONTAL;
    }
    NoiaFrame* resizee = noia_frame_resize_find_helper(self, type);

    // If nothing found, don't worry, that happens
    if (!resizee) {
        return;
    }

    // Resize using proper algorithm
    NoiaFrameParams* params =  noia_frame_get_params(resizee);
    if (params->type & NOIA_FRAME_TYPE_FLOATING) {
        noia_frame_resize_floating(resizee, direction, magnitude);
    } else if (params->type != NOIA_FRAME_TYPE_NONE) {
        noia_frame_resize_anchored(resizee, direction, magnitude);
    } else {
        LOG_WARN1("Resizing a None Frame!");
    }
}

//------------------------------------------------------------------------------

void noia_frame_move(NoiaFrame* self,
                     NoiaArgmand direction,
                     int magnitude)
{
    if (!self) {
        return;
    }

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

void noia_frame_jump(NoiaFrame* self,
                     NoiaArgmand direction NOIA_UNUSED,
                     int magnitude         NOIA_UNUSED)
{
    if (!self) {
        return;
    }
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

NoiaFrame* noia_frame_find_pointed(NoiaFrame* self,
                                   NoiaArgmand direction,
                                   int magnitude)
{
    assert(noia_argmand_is_directed(direction));

    // If magnitude is zero, this is the last step of recurency.
    if (magnitude == 0) {
        return self;
    }

    // Hangle negative magnitude.
    if (magnitude < 0) {
        direction = noia_argmand_reverse_directed(direction);
        magnitude = -magnitude;
    }

    // Find new frame whitch is farther.
    NoiaFrame* new_frame = NULL;
    int new_magnitude = magnitude;

    if (noia_frame_has_type(self->trunk, NOIA_FRAME_TYPE_VERTICAL)) {
        if (direction == NOIA_ARGMAND_N) {
            new_frame = noia_branch_get_previous(self);
        } else if (direction == NOIA_ARGMAND_S) {
            new_frame = noia_branch_get_next(self);
        }
    } else if (noia_frame_has_type(self->trunk, NOIA_FRAME_TYPE_HORIZONTAL)) {
        if (direction == NOIA_ARGMAND_W) {
            new_frame = noia_branch_get_previous(self);
        } else if (direction == NOIA_ARGMAND_E) {
            new_frame = noia_branch_get_next(self);
        }
    }

    // If there is nothing farther go higher. If it is, decrease magnitude.
    if (new_frame || direction == NOIA_ARGMAND_TRUNK) {
        new_magnitude = magnitude - 1;
    }
    if (!new_frame) {
        new_frame = self->trunk;
    }

    // Next recurency step if possible.
    if (!noia_frame_has_type(new_frame, NOIA_FRAME_TYPE_SPECIAL)) {
        return noia_frame_find_pointed(new_frame, direction, new_magnitude);
    } else {
        return NULL;
    }
}

//------------------------------------------------------------------------------

void noia_frame_log(NoiaFrame* self)
{
    noia_frame_log_internal(self, 0);
}

//------------------------------------------------------------------------------

