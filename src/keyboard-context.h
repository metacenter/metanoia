// file: keyboard-context.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_CONTEXT_H__
#define __NOIA_KEYBOARD_CONTEXT_H__

#include "utils-list.h"
#include "global-constants.h"

/// Context passed to keyboard binding handlers.
typedef struct {
    NoiaAction action;
    NoiaList* modes;
    int code;
    uint32_t modifiers;
    NoiaKeyState state;
} NoiaBindingContext;

/// Initializer of `NoiaBindingContext`.
#define NOIA_BINDING_CONTEXT_INIT { \
        { NOIA_ARGMAND_NONE, NOIA_ARGMAND_NONE, NOIA_FRAME_TYPE_NONE, 0 }, \
        NULL, 0, 0x0, NOIA_KEY_RELEASED }

#endif // __NOIA_KEYBOARD_CONTEXT_H__

