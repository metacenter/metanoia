// file: keyboard-argmand.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_ARGMAND_H__
#define __NOIA_KEYBOARD_ARGMAND_H__

#include "global-constants.h"

typedef struct {
    NoiaArgmandType type;
    int value;
} NoiaArgmand;

NoiaArgmand* noia_argmand_new(NoiaArgmandType type, int value);
void noia_argmand_free(NoiaArgmand* self);

#endif // __NOIA_KEYBOARD_ARGMAND_H__

