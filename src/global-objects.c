// file: global-objects.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "global-objects.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

#include "utils-log.h"

/// Free Int Object.
void aura_int_free(AuraIntObject* self) {
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(AuraIntObject));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Int Object.
AuraIntObject* aura_uint_create(uintptr_t value)
{
    AuraIntObject* self = malloc(sizeof(AuraIntObject));
    aura_object_initialize(&self->base, (AuraFreeFunc) aura_int_free);
    aura_object_ref(&self->base);
    self->uint = value;
    return self;
}

//------------------------------------------------------------------------------

/// Allocate and initialize Int Object.
AuraIntObject* aura_int_create(intptr_t value)
{
    AuraIntObject* self = malloc(sizeof(AuraIntObject));
    aura_object_initialize(&self->base, (AuraFreeFunc) aura_int_free);
    aura_object_ref(&self->base);
    self->sint = value;
    return self;
}

//------------------------------------------------------------------------------

/// Convenience for getting unsigned value and unrefing at once.
uintptr_t aura_uint_unref_get(AuraIntObject* self)
{
    uintptr_t result = 0;
    if (!self) {
        return result;
    }
    result = self->uint;
    aura_object_unref((AuraObject*) self);
    return result;
}

//------------------------------------------------------------------------------

/// Convenience for getting signed value and unrefing at once.
intptr_t aura_int_unref_get(AuraIntObject* self)
{
    intptr_t result = 0;
    if (!self) {
        return result;
    }
    result = self->sint;
    aura_object_unref((AuraObject*) self);
    return result;
}

//------------------------------------------------------------------------------

/// Free Key Object.
void aura_key_free(AuraKeyObject* self) {
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(AuraKeyObject));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Key Object.
AuraKeyObject* aura_key_create(unsigned time, int code, bool value)
{
    AuraKeyObject* self = malloc(sizeof(AuraKeyObject));
    aura_object_initialize(&self->base, (AuraFreeFunc) aura_key_free);
    aura_object_ref(&self->base);
    self->keydata.time = time;
    self->keydata.code = code;
    self->keydata.value = value;
    return self;
}

//------------------------------------------------------------------------------

