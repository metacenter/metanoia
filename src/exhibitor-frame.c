// file: exhibitor-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "utils-log.h"
#include "surface-manager.h"
#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

int noia_frame_compare_sid(NoiaFrame* frame, NoiaSurfaceId sid)
{
    NoiaSurfaceId frame_sid = noia_frame_get_params(frame)->sid;
    if (frame_sid < sid) {
        return -1;
    } else if (frame_sid > sid) {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------

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

NoiaFrame* noia_frame_new()
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
    return self;
}

//------------------------------------------------------------------------------

NoiaFrameParams* noia_frame_get_params(NoiaFrame* self)
{
    return self->base.data;
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

NoiaFrame* noia_frame_resize_find_helper(NoiaFrame* frame,
                                         NoiaFrameType type)
{
    if (noia_frame_get_params(frame)->type == NOIA_FRAME_TYPE_FLOATING
    ||  noia_frame_get_params(frame->trunk)->type == type) {
        return frame;
    }
    return noia_frame_resize_find_helper(frame->trunk, type);
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

void noia_frame_move_with_contents(NoiaFrame* self,
                                   NoiaPosition vector)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    // If frame is floating, update its position
    if (params->type == NOIA_FRAME_TYPE_FLOATING) {
        params->floating_position.x += vector.x;
        params->floating_position.y += vector.y;
    }

    // Update surfaces position
    NoiaSurfaceData* surface = noia_surface_get(params->sid);
    if (surface) {
        surface->position.x += vector.x;
        surface->position.y += vector.y;
    }

    // Move all subframes
    FOR_EACH_TWIG(self, branch) {
        noia_frame_move_with_contents(branch, vector);
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
    if (params->type != NOIA_FRAME_TYPE_FLOATING) {
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

void noia_frame_reconfigure(NOIA_UNUSED NoiaFrame* self,
                            NOIA_UNUSED NoiaArgmandType direction,
                            NOIA_UNUSED int magnitude)
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

void noia_frame_resize_floating(NoiaFrame* self,
                                NoiaArgmandType direction,
                                int magnitude)
{
    noia_frame_reconfigure(self, direction, magnitude);
}

//------------------------------------------------------------------------------

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
    if (params->type == NOIA_FRAME_TYPE_FLOATING) {
        noia_frame_resize_floating(resizee, direction, magnitude);
    } else if (params->type != NOIA_FRAME_TYPE_NONE) {
        noia_frame_resize_anchored(resizee, direction, magnitude);
    } else {
        LOG_WARN1("Resizing a None Frame!");
    }
}

//------------------------------------------------------------------------------

void noia_frame_jump(NoiaFrame* self,
                     NOIA_UNUSED NoiaArgmandType direction,
                     NOIA_UNUSED int magnitude)
{
    if (!self) {
        return;
    }

    /// @todo Implement noia_frame_jump
}

//------------------------------------------------------------------------------

void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop)
{
    if (!self || !pop || !pop->trunk || self == pop) {
        return;
    }

    if (noia_frame_get_params(pop->trunk)->type == NOIA_FRAME_TYPE_STACKED) {
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
                           (NoiaBranchCompare) noia_frame_compare_sid);
}

//------------------------------------------------------------------------------

