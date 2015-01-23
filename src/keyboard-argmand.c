// file: keyboard-argmand.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-argmand.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraArgmand* aura_argmand_new(AuraArgmandType type, int value)
{
    AuraArgmand* self = malloc(sizeof(AuraArgmand));
    if (!self) {
        LOG_ERROR("Memory allocation failed!");
        return self;
    }

    self->type = type;
    self->value = value;
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

