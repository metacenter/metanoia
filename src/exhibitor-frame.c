// file: exhibitor-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

AuraFrame* aura_frame_new()
{
    AuraFrame* self = aura_branch_new();

    self->data = malloc(sizeof(AuraFrameParams));
    if (!self) {
        LOG_ERROR("Memory allocation failed!");
        return NULL;
    }

    AuraFrameParams* params = self->data;
    memset(params, 0, sizeof(AuraFrameParams));
    params->type = AURA_FRAME_TYPE_NONE;
    params->sid = scInvalidSurfaceId;
    return self;
}

//------------------------------------------------------------------------------

void aura_frame_free(AuraFrame* self)
{
    if (!self) {
        return;
    }

    AuraFrameParams* params = self->data;
    if (params) {
        if (params->title) {
            free(params->title);
        }
        free(params);
    }
    free(self);
}

//------------------------------------------------------------------------------

inline void aura_frame_set_type(AuraFrame* self, AuraFrameType type)
{
    AuraFrameParams* params = self->data;
    params->type = type;
}

//------------------------------------------------------------------------------

inline bool aura_frame_is_workspace(AuraFrame* self)
{
    AuraFrameParams* params = self->data;
    return params->type == AURA_FRAME_TYPE_WORKSPACE;
}

//------------------------------------------------------------------------------

void aura_frame_resize_floating(self, direction, magnitude)
{
    /// @todo Implement aura_frame_resize_floating
}

//------------------------------------------------------------------------------

void aura_frame_resize_anchored(self, direction, magnitude)
{
    /// @todo Implement aura_frame_resize_anchored
}

//------------------------------------------------------------------------------

void aura_frame_resize(AuraFrame* self,
                       AuraArgmandType direction,
                       int magnitude)
{
    AuraFrameParams* params = self->data;
    if (params->type == AURA_FRAME_TYPE_FLOATING) {
        aura_frame_resize_floating(self, direction, magnitude);
    } else if (params->type != AURA_FRAME_TYPE_NONE) {
        aura_frame_resize_anchored(self, direction, magnitude);
    } else {
        LOG_WARN1("Resizing a None Frame!");
    }
}

//------------------------------------------------------------------------------

