// file: exhibitor-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "utils-log.h"
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
                            NoiaArgmandType direction,
                            int magnitude)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    if (params->sid != scInvalidSurfaceId) {
        NoiaSurfaceData* surface = noia_surface_get(params->sid);
        if (surface) {
            if (direction == NOIA_ARGMAND_N || direction == NOIA_ARGMAND_S) {
                if (surface->desired_size.height == 0) {
                    surface->desired_size.height =
                                                 surface->requested_size.height;
                }
                surface->desired_size.height += magnitude;
            }
            if (direction == NOIA_ARGMAND_E || direction == NOIA_ARGMAND_W) {
                if (surface->desired_size.width == 0) {
                    surface->desired_size.width = surface->requested_size.width;
                }
                surface->desired_size.width += magnitude;
            }
            noia_event_signal_emit_int(SIGNAL_SURFACE_RECONFIGURED,params->sid);
        }
    }


    /// @todo Implement noia_frame_reconfigure for twigs.
}

//------------------------------------------------------------------------------

/// Resize floating frame.
void noia_frame_resize_floating(NoiaFrame* self,
                                NoiaArgmandType direction,
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
                                NoiaArgmandType direction,
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
                   params->type, params->sid, chain_len(self->twigs));

    FOR_EACH_TWIG(self, twig) {
        noia_frame_log_internal(twig, level+1);
    }

    if (level == 0) {
        noia_log_end();
    }
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaFrame* noia_frame_create(NoiaPosition position, NoiaSize size)
{
    NoiaFrame* self = noia_branch_new();

    self->base.data = malloc(sizeof(NoiaFrameParams));
    if (!self->base.data) {
        LOG_ERROR("Memory allocation failed!");
        return NULL;
    }

    NoiaFrameParams* params = self->base.data;
    memset(params, 0, sizeof(NoiaFrameParams));
    params->type = NOIA_FRAME_TYPE_NONE;
    params->sid = scInvalidSurfaceId;
    params->pos = position;
    params->size = size;
    return self;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_create_child(NoiaFrame* self, NoiaFrameType type)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    if (!(params->type & NOIA_FRAME_TYPE_STACKED)) {
        return NULL;
    }

    NoiaFrame* frame = noia_frame_create(params->pos, params->size);
    noia_frame_set_type(frame, type);

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

void noia_frame_init_as_workspace(NoiaFrame* self)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    params->type = NOIA_FRAME_TYPE_WORKSPACE;
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

void noia_frame_set_surface(NoiaFrame* self, NoiaSurfaceId sid)
{
    noia_frame_get_params(self)->sid = sid;
    NoiaSurfaceData* surface = noia_surface_get(sid);
    if (!surface) {
        return;
    }

    // Reconfiguration
    if (surface->requested_size.width  != surface->desired_size.width
    ||  surface->requested_size.height != surface->desired_size.height) {
        surface->desired_size.width  = surface->requested_size.width;
        surface->desired_size.height = surface->requested_size.height;
        noia_event_signal_emit_int(SIGNAL_SURFACE_RECONFIGURED, sid);
    }
}

//------------------------------------------------------------------------------

void noia_frame_set_type(NoiaFrame* self, NoiaFrameType type)
{
    noia_frame_get_params(self)->type = type;
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
                       NoiaArgmandType direction,
                       int magnitude)
{
    if (!self) {
        return;
    }

    // Find adequate frame for resize
    NoiaFrameType type = 0x0;
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
                     NoiaArgmandType direction,
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
                     NOIA_UNUSED NoiaArgmandType direction,
                     NOIA_UNUSED int magnitude)
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

void noia_frame_log(NoiaFrame* self)
{
    noia_frame_log_internal(self, 0);
}

//------------------------------------------------------------------------------

