// file: keyboard-context.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_KEYBOARD_CONTEXT_H
#define NOIA_KEYBOARD_CONTEXT_H

#include "utils-list.h"
#include "global-objects.h"
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
        {{0, NULL}, NOIA_ARGMAND_NONE, NOIA_ARGMAND_NONE, NULL, 0 }, \
        NULL, 0, 0x0, NOIA_KEY_RELEASED }

#endif // NOIA_KEYBOARD_CONTEXT_H

