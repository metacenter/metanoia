// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "global-objects.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

#include "utils-log.h"

/// Free Int Object.
void noia_int_destroy(NoiaIntObject* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(*self));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Int Object.
NoiaIntObject* noia_uint_create(uintptr_t value)
{
    NoiaIntObject* self = malloc(sizeof(*self));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_int_destroy);
    noia_object_ref(&self->base);
    self->uint = value;
    return self;
}

//------------------------------------------------------------------------------

/// Allocate and initialize Int Object.
NoiaIntObject* noia_int_create(intptr_t value)
{
    NoiaIntObject* self = malloc(sizeof(*self));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_int_destroy);
    noia_object_ref(&self->base);
    self->sint = value;
    return self;
}

//------------------------------------------------------------------------------

/// Convenience for getting unsigned value and unrefing at once.
uintptr_t noia_uint_unref_get(NoiaIntObject* self)
{
    uintptr_t result = 0;
    NOIA_ENSURE(self, return result);
    result = self->uint;
    noia_object_unref((NoiaObject*) self);
    return result;
}

//------------------------------------------------------------------------------

/// Convenience for getting signed value and unrefing at once.
intptr_t noia_int_unref_get(NoiaIntObject* self)
{
    uintptr_t result = 0;
    NOIA_ENSURE(self, return result);
    result = self->sint;
    noia_object_unref((NoiaObject*) self);
    return result;
}

//------------------------------------------------------------------------------

/// Free Key Object.
void noia_key_destroy(NoiaKeyObject* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(*self));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Key Object.
NoiaKeyObject* noia_key_create(unsigned time, int code, NoiaKeyState value)
{
    NoiaKeyObject* self = malloc(sizeof(*self));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_key_destroy);
    noia_object_ref(&self->base);
    self->keydata.time = time;
    self->keydata.code = code;
    self->keydata.value = value;
    return self;
}

//------------------------------------------------------------------------------

/// Free Button Object.
void noia_button_destroy(NoiaButtonObject* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(*self));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Button Object.
NoiaButtonObject* noia_button_create(unsigned time, int code, bool value)
{
    NoiaButtonObject* self = malloc(sizeof(*self));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_button_destroy);
    noia_object_ref(&self->base);
    self->buttondata.time = time;
    self->buttondata.code = code;
    self->buttondata.value = value;
    return self;
}

//------------------------------------------------------------------------------

/// Free Motion Object.
void noia_motion_destroy(NoiaMotionObject* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(*self));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Motion Object
NoiaMotionObject* noia_motion_create(NoiaSurfaceId sid, NoiaPosition pos)
{
    NoiaMotionObject* self = malloc(sizeof(*self));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_motion_destroy);
    noia_object_ref(&self->base);
    self->sid = sid;
    self->pos = pos;
    return self;
}

//------------------------------------------------------------------------------

/// Free Axis Object.
void noia_axis_destroy(NoiaAxisObject* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(*self));
    free(self);
}

//------------------------------------------------------------------------------

/// Allocate and initialize Axis Object.
NoiaAxisObject* noia_axis_create(double h, double v, int hd, int vd)
{
    NoiaAxisObject* self= malloc(sizeof(*self));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_axis_destroy);
    noia_object_ref(&self->base);
    self->axisdata.h = h;
    self->axisdata.v = v;
    self->axisdata.hd = hd;
    self->axisdata.vd = vd;
    return self;
}

//------------------------------------------------------------------------------

/// Create a command.
NoiaCommand* noia_command_create(void)
{
    NoiaCommand* self = malloc(sizeof(*self));
    noia_object_initialize(&self->base, (NoiaFreeFunc) noia_command_destroy);
    self->str = NULL;
    noia_command_clean(self);
    return self;
}

//------------------------------------------------------------------------------

/// Free a command.
void noia_command_destroy(NoiaCommand* self)
{
    NOIA_ENSURE(self, return);
    noia_command_clean(self);
    free(self);
}

//------------------------------------------------------------------------------

/// Clean a command.
void noia_command_clean(NoiaCommand* self)
{
    self->action    = NOIA_ACTION_NONE;
    self->direction = NOIA_DIRECTION_NONE;
    self->magnitude = 0;
    if (self->str) {
        free(self->str);
        self->str = NULL;
    }
}

//------------------------------------------------------------------------------

/// Copy a commnad.
NoiaCommand* noia_command_copy(NoiaCommand* self)
{
    NoiaCommand* copy = noia_command_create();
    copy->action    = self->action;
    copy->direction = self->direction;
    copy->magnitude = self->magnitude;
    if (self->str) {
        copy->str = strdup(self->str);
    } else {
        copy->str = NULL;
    }
    return copy;
}

//------------------------------------------------------------------------------

