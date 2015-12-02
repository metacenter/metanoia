// file: utils-object.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_OBJECT_H
#define NOIA_OBJECT_H

#include "global-constants.h"

/// Reference count implementation.
typedef struct {
    int refcount;
    NoiaFreeFunc free;
} NoiaObject;

void noia_object_initialize(NoiaObject* self, NoiaFreeFunc free);

void noia_object_ref(NoiaObject* self);

void noia_object_unref(NoiaObject* self);

#endif // NOIA_OBJECT_H

