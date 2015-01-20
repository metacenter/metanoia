// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_FRAME_H__
#define __AURA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"
#include "global-constants.h"

typedef enum {
    AURA_FRAME_TYPE_NONE            = 0x0,
    AURA_FRAME_TYPE_WORKSPACE       = 0x1,
    AURA_FRAME_TYPE_HORIZONTAL_TREE = 0x2,
    AURA_FRAME_TYPE_VERTICAL_TREE   = 0x4,
    AURA_FRAME_TYPE_FLOATING        = 0x8,
} AuraFrameType;

typedef AuraBranch AuraFrame;

typedef struct {
    AuraFrameType type;
    char* title;
    SurfaceId sid;
    AuraPosition floating_position;
} AuraFrameParams;

AuraFrame* aura_frame_new();
void aura_frame_free(AuraFrame* self);

void aura_frame_set_surface(AuraFrame* self, SurfaceId sid);
void aura_frame_set_type(AuraFrame* self, AuraFrameType type);
bool aura_frame_is_workspace(AuraFrame* self);

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

#endif // __AURA_EXHIBITOR_FRAME_H__

