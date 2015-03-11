// file: utils-object.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_OBJECT_H__
#define __NOIA_OBJECT_H__

#include "global-constants.h"

/// Reference count implementation.
typedef struct {
    int refcount;
    NoiaFreeFunc free;
} NoiaObject;

void noia_object_initialize(NoiaObject* self, NoiaFreeFunc free);

void noia_object_ref(NoiaObject* self);

void noia_object_unref(NoiaObject* self);

#endif // __NOIA_OBJECT_H__

