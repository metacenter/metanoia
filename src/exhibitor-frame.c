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

    memset(self->data, 0, sizeof(AuraFrameParams));
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

