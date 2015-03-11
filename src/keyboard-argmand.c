// file: keyboard-argmand.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-argmand.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

NoiaArgmand* noia_argmand_new(NoiaArgmandType type, int value)
{
    NoiaArgmand* self = malloc(sizeof(NoiaArgmand));
    if (!self) {
        LOG_ERROR("Memory allocation failed!");
        return self;
    }

    self->type = type;
    self->value = value;
    return self;
}

//------------------------------------------------------------------------------

void noia_argmand_free(NoiaArgmand* self)
{
    if (!self) {
        return;
    }
    free(self);
}

//------------------------------------------------------------------------------

