// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_FRAME_H__
#define __AURA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"
#include "global-constants.h"

typedef enum {
    AURA_FRAME_TYPE_NONE = 0,
    AURA_FRAME_TYPE_WORKSPACE,
    AURA_FRAME_TYPE_HORIZONTAL_TREE,
    AURA_FRAME_TYPE_VERTICAL_TREE,
    AURA_FRAME_TYPE_FLOATING,
} AuraFrameType;

typedef AuraBranch AuraFrame;

typedef struct {
    AuraFrameType type;
    char* title;
    SurfaceId sid;
} AuraFrameParams;

AuraFrame* aura_frame_new();
void aura_frame_free(AuraFrame* self);

void aura_frame_set_type(AuraFrame* self, AuraFrameType type);
bool aura_frame_is_workspace(AuraFrame* self);

void aura_frame_resize(AuraFrame* self,
                       AuraArgmandType direction,
                       int magnitude);

#endif // __AURA_EXHIBITOR_FRAME_H__

