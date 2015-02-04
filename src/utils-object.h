// file: utils-object.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_OBJECT_H__
#define __AURA_OBJECT_H__

#include "global-constants.h"

/// Reference count implementation.
typedef struct {
    int refcount;
    AuraFreeFunc free;
} AuraObject;

void aura_object_initialize(AuraObject* self, AuraFreeFunc free);

void aura_object_ref(AuraObject* self);

void aura_object_unref(AuraObject* self);

#endif // __AURA_OBJECT_H__

