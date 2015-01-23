// file: keyboard-binding.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-binding.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraBinding* aura_binding_new()
{
    AuraBinding* self = malloc(sizeof(AuraBinding));
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

void aura_binding_free(AuraBinding* self)
{
    if (!self) {
        return;
    }
    free(self);
}

//------------------------------------------------------------------------------

AuraBinding* aura_binding_copy(const AuraBinding* self)
{
    AuraBinding* copy = aura_binding_new();
    copy->code      = self->code;
    copy->modifiers = self->modifiers;
    copy->execute   = self->execute;
    return copy;
}

//------------------------------------------------------------------------------

