// file: utils-object.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-object.h"

//------------------------------------------------------------------------------

/// Initialize object by setting free function and zeroing reference count.
void aura_object_initialize(AuraObject* self, AuraFreeFunc free)
{
    if (!self) {
        return;
    }

    self->refcount = 0;
    self->free = free;
}

//------------------------------------------------------------------------------

/// Increase reference count by one.
void aura_object_ref(AuraObject* self)
{
    if (!self) {
        return;
    }

    self->refcount += 1;
}

//------------------------------------------------------------------------------

/// Decrease reference count by one and free object if nothing references it.
void aura_object_unref(AuraObject* self)
{
    if (!self) {
        return;
    }

    self->refcount -= 1;

    if (self->refcount < 1) {
        self->free(self);
    }
}

//------------------------------------------------------------------------------

