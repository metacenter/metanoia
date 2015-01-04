// file: keyboard-argmand.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_ARGMAND_H__
#define __AURA_KEYBOARD_ARGMAND_H__

#include "utils-chain.h"

#include <stdint.h>

typedef void (*AuraArgmandExecuteFunc) (Chain*);

typedef struct {
    int code;
    uint32_t modifiers;
    AuraArgmandExecuteFunc execute;
} AuraArgmand;

void aura_argmand_free(AuraArgmand* self);

AuraArgmand* aura_argmand_copy(const AuraArgmand* self);

#endif // __AURA_KEYBOARD_ARGMAND_H__

