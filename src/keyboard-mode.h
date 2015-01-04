// file: keyboard-mode.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_MODE_H__
#define __AURA_KEYBOARD_MODE_H__

#include "keyboard-argmand.h"
#include "global-constants.h"

typedef struct {
    AuraModeEnum modeid;
    void* argmands;
    int active;
} AuraMode;

AuraMode* aura_mode_new(AuraModeEnum modeid);

void aura_mode_add_argmand(AuraMode* self, const AuraArgmand* argmand);

AuraArgmand* aura_mode_find_argmand(AuraMode* self,
                                    int code,
                                    uint32_t Auramodifiers);

#endif // __AURA_KEYBOARD_MODE_H__

