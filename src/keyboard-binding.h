// file: keyboard-binding.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_KEYBOARD_BINDING_H__
#define __AURA_KEYBOARD_BINDING_H__

#include "utils-chain.h"

#include <stdint.h>

typedef void (*AuraBindingSimpleExecuteFunc) (Chain*);
typedef void (*AuraBindingExecuteFunc) (Chain*, int, uint32_t, AuraKeyState);

typedef struct {
    int code;
    uint32_t modifiers;
    /// Workaround to easily define bindings in configuration without cast
    union {
        AuraBindingSimpleExecuteFunc execute_simple;
        AuraBindingExecuteFunc execute;
    };
} AuraBinding;

void aura_binding_free(AuraBinding* self);

AuraBinding* aura_binding_copy(const AuraBinding* self);

#endif // __AURA_KEYBOARD_BINDING_H__

