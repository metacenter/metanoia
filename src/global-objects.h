// file: global-objects.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_GLOBAL_OBJECTS_H__
#define __AURA_GLOBAL_OBJECTS_H__

#include "global-types.h"
#include "utils-object.h"

//------------------------------------------------------------------------------

/// Object wrapping integer value.
/// Integer with reference counting.
typedef struct {
    AuraObject base;
    union {
        uintptr_t uint;
        intptr_t sint;
    };
} AuraIntObject;

AuraIntObject* aura_uint_create(uintptr_t value);
AuraIntObject* aura_int_create(intptr_t value);

uintptr_t aura_uint_unref_get(AuraIntObject* self);
intptr_t aura_int_unref_get(AuraIntObject* self);

//------------------------------------------------------------------------------

/// Key event object.
/// KeyData structure with reference counting.
typedef struct {
    AuraObject base;
    AuraKeyData keydata;
} AuraKeyObject;

AuraKeyObject* aura_key_create(unsigned time, int code, bool value);

//------------------------------------------------------------------------------

#endif // __AURA_GLOBAL_OBJECTS_H__

