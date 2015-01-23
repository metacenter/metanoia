// file: exhibitor-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "utils-log.h"
#include "surface-manager.h"
#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

int aura_frame_compare_sid(AuraFrame* frame, SurfaceId sid)
{
    SurfaceId frame_sid = aura_frame_get_params(frame)->sid;
    if (frame_sid < sid) {
        return -1;
    } else if (frame_sid > sid) {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------

AuraFrame* aura_frame_new()
{
    AuraFrame* self = aura_branch_new();

    self->link.data = malloc(sizeof(AuraFrameParams));
    if (!self->link.data) {
        LOG_ERROR("Memory allocation failed!");
        return NULL;
    }

    AuraFrameParams* params = self->link.data;
    memset(params, 0, sizeof(AuraFrameParams));
    params->type = AURA_FRAME_TYPE_NONE;
    params->sid = scInvalidSurfaceId;
    return self;
}

//------------------------------------------------------------------------------

AuraFrameParams* aura_frame_get_params(AuraFrame* self)
{
    return self->link.data;
}

//------------------------------------------------------------------------------

void aura_frame_free(AuraFrame* self)
{
    if (!self) {
        return;
    }

    AuraFrameParams* params = aura_frame_get_params(self);
    if (params) {
        if (params->title) {
            free(params->title);
        }
        free(params);
    }
    free(self);
}

//------------------------------------------------------------------------------

AuraFrame* aura_frame_resize_find_helper(AuraFrame* frame,
                                         AuraFrameType type)
{
    if (aura_frame_get_params(frame)->type == AURA_FRAME_TYPE_FLOATING
    ||  aura_frame_get_params(frame->trunk)->type == type) {
        return frame;
    }
    return aura_frame_resize_find_helper(frame->trunk, type);
}

//------------------------------------------------------------------------------

void aura_frame_set_surface(AuraFrame* self, SurfaceId sid)
{
    aura_frame_get_params(self)->sid = sid;
}

//------------------------------------------------------------------------------

void aura_frame_set_type(AuraFrame* self, AuraFrameType type)
{
    aura_frame_get_params(self)->type = type;
}

//------------------------------------------------------------------------------

void aura_frame_move_with_contents(AuraFrame* self,
                                   AuraPosition vector)
{
    AuraFrameParams* params = aura_frame_get_params(self);

    // If frame is floating, update its position
    if (params->type == AURA_FRAME_TYPE_FLOATING) {
        params->floating_position.x += vector.x;
        params->floating_position.y += vector.y;
    }

    // Update surfaces position
    AuraSurfaceData* surface_data = aura_surface_get(params->sid);
    if (surface_data) {
        surface_data->position.x += vector.x;
        surface_data->position.y += vector.y;
    }

    // Move all subframes
    FOR_EACH_TWIG(self, branch) {
        aura_frame_move_with_contents(branch, vector);
    }
}

//------------------------------------------------------------------------------

void aura_frame_move(AuraFrame* self,
                     AuraArgmandType direction,
                     int magnitude)
{
    if (!self) {
        return;
    }

    AuraFrameParams* params = aura_frame_get_params(self);
    if (params->type != AURA_FRAME_TYPE_FLOATING) {
        return;
    }

    AuraPosition vector = {0, 0};
    if (direction == AURA_ARGMAND_S) {
        vector.y += magnitude;
    }
    if (direction == AURA_ARGMAND_N) {
        vector.y -= magnitude;
    }
    if (direction == AURA_ARGMAND_E) {
        vector.x += magnitude;
    }
    if (direction == AURA_ARGMAND_W) {
        vector.x -= magnitude;
    }

    aura_frame_move_with_contents(self, vector);
}

//------------------------------------------------------------------------------

void aura_frame_reconfigure(AURA_UNUSED AuraFrame* self,
                            AURA_UNUSED AuraArgmandType direction,
                            AURA_UNUSED int magnitude)
{
    /// @todo Implement aura_frame_reconfigure
    AuraFrameParams* params = aura_frame_get_params(self);
    if (params->sid != scInvalidSurfaceId) {
        AuraSurfaceData* surface_data = aura_surface_get(params->sid);
        if (surface_data) {
            if (direction == AURA_ARGMAND_N || direction == AURA_ARGMAND_S) {
                surface_data->desired_size.height += magnitude;
            }
            if (direction == AURA_ARGMAND_E || direction == AURA_ARGMAND_W) {
                surface_data->desired_size.width += magnitude;
            }
            aura_event_signal_emit(SIGNAL_SURFACE_RECONFIGURED,
                                   (void*) params->sid);
        }
    }
}

//------------------------------------------------------------------------------

void aura_frame_resize_floating(AuraFrame* self,
                                AuraArgmandType direction,
                                int magnitude)
{
    aura_frame_reconfigure(self, direction, magnitude);
}

//------------------------------------------------------------------------------

void aura_frame_resize_anchored(AuraFrame* self,
                                AuraArgmandType direction,
                                int magnitude)
{
    if (direction == AURA_ARGMAND_N || direction == AURA_ARGMAND_W) {
        if (self->link.prev) {
            aura_frame_reconfigure(self, direction, magnitude);
            aura_frame_reconfigure((AuraFrame*) self->link.prev,
                                   direction == AURA_ARGMAND_N ?
                                   AURA_ARGMAND_W : AURA_ARGMAND_N,
                                   -magnitude);
        }
    }

    if (direction == AURA_ARGMAND_S || direction == AURA_ARGMAND_E) {
        if (self->link.next) {
            aura_frame_reconfigure(self, direction, magnitude);
            aura_frame_reconfigure((AuraFrame*) self->link.next,
                                   direction == AURA_ARGMAND_S ?
                                   AURA_ARGMAND_E : AURA_ARGMAND_N,
                                   -magnitude);
        }
    }
}

//------------------------------------------------------------------------------

void aura_frame_resize(AuraFrame* self,
                       AuraArgmandType direction,
                       int magnitude)
{
    if (!self) {
        return;
    }

    // Find adequate frame for resize
    AuraFrameType type = 0x0;
    if (direction == AURA_ARGMAND_N || direction == AURA_ARGMAND_S) {
        type = AURA_FRAME_TYPE_VERTICAL;
    } else if (direction == AURA_ARGMAND_E || direction == AURA_ARGMAND_W) {
        type = AURA_FRAME_TYPE_HORIZONTAL;
    }
    AuraFrame* resizee = aura_frame_resize_find_helper(self, type);

    // If nothing found, don't worry, that happens
    if (!resizee) {
        return;
    }

    // Resize using proper algorithm
    AuraFrameParams* params =  aura_frame_get_params(resizee);
    if (params->type == AURA_FRAME_TYPE_FLOATING) {
        aura_frame_resize_floating(resizee, direction, magnitude);
    } else if (params->type != AURA_FRAME_TYPE_NONE) {
        aura_frame_resize_anchored(resizee, direction, magnitude);
    } else {
        LOG_WARN1("Resizing a None Frame!");
    }
}

//------------------------------------------------------------------------------

void aura_frame_jump(AuraFrame* self,
                     AURA_UNUSED AuraArgmandType direction,
                     AURA_UNUSED int magnitude)
{
    if (!self) {
        return;
    }

    /// @todo Implement aura_frame_jump
}

//------------------------------------------------------------------------------

void aura_frame_pop_recursively(AuraFrame* self, AuraFrame* pop)
{
    if (!self || !pop || !pop->trunk || self == pop) {
        return;
    }

    if (aura_frame_get_params(pop->trunk)->type == AURA_FRAME_TYPE_STACKED) {
        AuraBranch* trunk = pop->trunk;
        aura_branch_remove(trunk, pop);
        aura_branch_append(trunk, pop);
    }

    aura_frame_pop_recursively(self, pop->trunk);
}

//------------------------------------------------------------------------------

AuraFrame* aura_frame_find_with_sid(AuraFrame* self, SurfaceId sid)
{
    return aura_branch_find(self, (void*) sid,
                           (AuraBranchCompare) aura_frame_compare_sid);
}

//------------------------------------------------------------------------------

