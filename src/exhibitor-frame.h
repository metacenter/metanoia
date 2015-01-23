// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_FRAME_H__
#define __AURA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"

typedef enum {
    AURA_FRAME_TYPE_NONE       = 0x00,
    AURA_FRAME_TYPE_LEAF       = 0x01,
    AURA_FRAME_TYPE_STACKED    = 0x02,
    AURA_FRAME_TYPE_HORIZONTAL = 0x04,
    AURA_FRAME_TYPE_VERTICAL   = 0x08,
    AURA_FRAME_TYPE_FLOATING   = 0x10,
} AuraFrameType;

/// @todo Unit tests for AuraFrame
typedef AuraBranch AuraFrame;

typedef struct {
    AuraFrameType type;
    char* title;
    AuraSurfaceId sid;
    AuraPosition floating_position;
} AuraFrameParams;

AuraFrame* aura_frame_new();
void aura_frame_free(AuraFrame* self);

void aura_frame_set_surface(AuraFrame* self, AuraSurfaceId sid);
void aura_frame_set_type(AuraFrame* self, AuraFrameType type);

AuraFrameParams* aura_frame_get_params(AuraFrame* self);

void aura_frame_resize(AuraFrame* self,
                       AuraArgmandType direction,
                       int magnitude);

void aura_frame_move(AuraFrame* self,
                     AuraArgmandType direction,
                     int magnitude);

void aura_frame_jump(AuraFrame* self,
                     AuraArgmandType direction,
                     int magnitude);

void aura_frame_pop_recursively(AuraFrame* self, AuraFrame* pop);

AuraFrame* aura_frame_find_with_sid(AuraFrame* self, AuraSurfaceId sid);

#endif // __AURA_EXHIBITOR_FRAME_H__

