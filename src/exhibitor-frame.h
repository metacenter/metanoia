// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_FRAME_H__
#define __AURA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"

typedef AuraBranch AuraFrame;

typedef struct {
    char* title;
} AuraFrameParams;

AuraFrame* aura_frame_new();
void aura_frame_free(AuraFrame* self);

#endif // __AURA_EXHIBITOR_FRAME_H__

