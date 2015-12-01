// file: keyboard-binding.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-binding.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

NoiaBinding* noia_binding_new()
{
    NoiaBinding* self = malloc(sizeof(NoiaBinding));
    if (!self) {
        LOG_ERROR("Memory allocation failed!");
        return self;
    }

    self->code = 0;
    self->modifiers = 0;
    self->execute = NULL;
    return self;
}

//------------------------------------------------------------------------------

void noia_binding_free(NoiaBinding* self)
{
    if (!self) {
        return;
    }
    free(self);
}

//------------------------------------------------------------------------------

NoiaBinding* noia_binding_copy(const NoiaBinding* self)
{
    NoiaBinding* copy = noia_binding_new();
    copy->code      = self->code;
    copy->modifiers = self->modifiers;
    copy->execute   = self->execute;
    return copy;
}

//------------------------------------------------------------------------------

int noia_binding_compare(const NoiaBinding* binding1,
                         const NoiaBinding* binding2)
{
    if (binding1->modifiers < binding2->modifiers) return -1;
    if (binding1->modifiers > binding2->modifiers) return  1;

    if (binding1->code < binding2->code) return -1;
    if (binding1->code > binding2->code) return  1;

    return 0;
}

//------------------------------------------------------------------------------

