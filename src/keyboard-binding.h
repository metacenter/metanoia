// file: keyboard-binding.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_BINDING_H__
#define __NOIA_KEYBOARD_BINDING_H__

#include "utils-list.h"

#include <stdint.h>

typedef void (*NoiaBindingSimpleExecuteFunc) (NoiaList*);
typedef void (*NoiaBindingExecuteFunc) (NoiaList*, int, uint32_t, NoiaKeyState);

typedef struct {
    int code;
    uint32_t modifiers;
    /// Workaround to easily define bindings in configuration without cast
    union {
        NoiaBindingSimpleExecuteFunc execute_simple;
        NoiaBindingExecuteFunc execute;
    };
} NoiaBinding;

void noia_binding_free(NoiaBinding* self);

NoiaBinding* noia_binding_copy(const NoiaBinding* self);

#endif // __NOIA_KEYBOARD_BINDING_H__

