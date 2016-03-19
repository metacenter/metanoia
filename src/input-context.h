// file: input-context.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_INPUT_CONTEXT_H
#define NOIA_INPUT_CONTEXT_H

#include "utils-list.h"
#include "global-objects.h"
#include "global-constants.h"

/// Context passed to input binding handlers.
typedef struct {
    NoiaAction* action;
    int code;
    uint32_t modifiers;
    NoiaKeyState state;
} NoiaInputContext;

/// Constructor of `NoiaInputContext`.
NoiaInputContext* noia_input_context_create(void);

/// Destructor of `NoiaInputContext`.
void noia_input_context_destroy(NoiaInputContext* self);

#endif // NOIA_INPUT_CONTEXT_H

