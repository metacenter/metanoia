// file: keyboard-argmand.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-argmand.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraArgmand* aura_argmand_new()
{
    AuraArgmand* self = malloc(sizeof(AuraArgmand));
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

void aura_argmand_free(AuraArgmand* self)
{
    if (!self) {
        return;
    }
    free(self);
}

//------------------------------------------------------------------------------

AuraArgmand* aura_argmand_copy(const AuraArgmand* self)
{
    AuraArgmand* copy = aura_argmand_new();
    copy->code      = self->code;
    copy->modifiers = self->modifiers;
    copy->execute   = self->execute;
    return copy;
}

//------------------------------------------------------------------------------

