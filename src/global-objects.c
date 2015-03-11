// file: global-objects.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "global-objects.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

#include "utils-log.h"

/// Free Int Object.
void noia_int_free(NoiaIntObject* self) {
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(NoiaIntObject));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Int Object.
NoiaIntObject* noia_uint_create(uintptr_t value)
{
    NoiaIntObject* self = malloc(sizeof(NoiaIntObject));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_int_free);
    noia_object_ref(&self->base);
    self->uint = value;
    return self;
}

//------------------------------------------------------------------------------

/// Allocate and initialize Int Object.
NoiaIntObject* noia_int_create(intptr_t value)
{
    NoiaIntObject* self = malloc(sizeof(NoiaIntObject));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_int_free);
    noia_object_ref(&self->base);
    self->sint = value;
    return self;
}

//------------------------------------------------------------------------------

/// Convenience for getting unsigned value and unrefing at once.
uintptr_t noia_uint_unref_get(NoiaIntObject* self)
{
    uintptr_t result = 0;
    if (!self) {
        return result;
    }
    result = self->uint;
    noia_object_unref((NoiaObject*) self);
    return result;
}

//------------------------------------------------------------------------------

/// Convenience for getting signed value and unrefing at once.
intptr_t noia_int_unref_get(NoiaIntObject* self)
{
    intptr_t result = 0;
    if (!self) {
        return result;
    }
    result = self->sint;
    noia_object_unref((NoiaObject*) self);
    return result;
}

//------------------------------------------------------------------------------

/// Free Key Object.
void noia_key_free(NoiaKeyObject* self) {
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(NoiaKeyObject));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Key Object.
NoiaKeyObject* noia_key_create(unsigned time, int code, bool value)
{
    NoiaKeyObject* self = malloc(sizeof(NoiaKeyObject));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_key_free);
    noia_object_ref(&self->base);
    self->keydata.time = time;
    self->keydata.code = code;
    self->keydata.value = value;
    return self;
}

//------------------------------------------------------------------------------

/// Free Button Object.
void noia_button_free(NoiaButtonObject* self) {
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(NoiaButtonObject));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Button Object.
NoiaButtonObject* noia_button_create(unsigned time, int code, bool value)
{
    NoiaButtonObject* self = malloc(sizeof(NoiaButtonObject));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_button_free);
    noia_object_ref(&self->base);
    self->buttondata.time = time;
    self->buttondata.code = code;
    self->buttondata.value = value;
    return self;
}

//------------------------------------------------------------------------------

/// Free Motion Object.
void noia_motion_free(NoiaMotionObject* self) {
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(NoiaMotionObject));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Motion Object
NoiaMotionObject* noia_motion_create(NoiaSurfaceId sid, NoiaPosition pos)
{
    NoiaMotionObject* self = malloc(sizeof(NoiaMotionObject));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_motion_free);
    self->sid = sid;
    self->pos = pos;
    return self;
}

//------------------------------------------------------------------------------

