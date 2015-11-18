// file: keyboard-argmand.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-argmand.h"
#include "utils-log.h"
#include "global-macros.h"

#include <malloc.h>

//------------------------------------------------------------------------------

NoiaArgmand* noia_argmand_new()
{
    NoiaArgmand* self = malloc(sizeof(NoiaArgmand));
    assert(self);

    noia_argmand_init(self, NOIA_ARGMAND_NONE, 0);
    return self;
}

//------------------------------------------------------------------------------

void noia_argmand_init(NoiaArgmand* self, NoiaArgmandType type, int value)
{
    assert(self);
    self->type = type;
    self->value = value;
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

bool noia_argmand_type_is_directed(NoiaArgmandType type)
{
    switch (type) {
    case NOIA_ARGMAND_N:
    case NOIA_ARGMAND_E:
    case NOIA_ARGMAND_S:
    case NOIA_ARGMAND_W:
    case NOIA_ARGMAND_BACK:
    case NOIA_ARGMAND_FORWARD:
    case NOIA_ARGMAND_BEGIN:
    case NOIA_ARGMAND_END:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------

bool noia_argmand_type_is_actionable(NoiaArgmandType type)
{
    switch (type) {
    case NOIA_ARGMAND_FOCUS:
    case NOIA_ARGMAND_MOVE:
    case NOIA_ARGMAND_JUMP:
    case NOIA_ARGMAND_RESIZE:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------

