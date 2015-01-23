// file: keyboard-argmand.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_ARGMAND_H__
#define __AURA_KEYBOARD_ARGMAND_H__

#include "global-constants.h"

typedef struct {
    AuraArgmandType type;
    int value;
} AuraArgmand;

AuraArgmand* aura_argmand_new(AuraArgmandType type, int value);
void aura_argmand_free(AuraArgmand* self);

#endif // __AURA_KEYBOARD_ARGMAND_H__

