// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_GLOBAL_OBJECTS_H
#define NOIA_GLOBAL_OBJECTS_H

#include "utils-object.h"

//------------------------------------------------------------------------------

/// Object wrapping integer value.
/// Integer with reference counting.
typedef struct {
    NoiaObject base;
    union {
        uintptr_t uint;
        intptr_t sint;
    };
} NoiaIntObject;

NoiaIntObject* noia_uint_create(uintptr_t value);
NoiaIntObject* noia_int_create(intptr_t value);

uintptr_t noia_uint_unref_get(NoiaIntObject* self);
intptr_t noia_int_unref_get(NoiaIntObject* self);

//------------------------------------------------------------------------------

/// Key event object.
/// KeyData structure with reference counting.
typedef struct {
    NoiaObject base;
    NoiaKeyData keydata;
} NoiaKeyObject;

NoiaKeyObject* noia_key_create(unsigned time, int code, NoiaKeyState value);

//------------------------------------------------------------------------------

/// Button event object.
/// ButtonData structure with reference counting.
typedef struct {
    NoiaObject base;
    NoiaButtonData buttondata;
} NoiaButtonObject;

NoiaButtonObject* noia_button_create(unsigned time, int code, bool value);

//------------------------------------------------------------------------------

/// Pointer motion object.
typedef struct {
    NoiaObject base;
    NoiaSurfaceId sid;
    NoiaPosition pos;
} NoiaMotionObject;

NoiaMotionObject* noia_motion_create(NoiaSurfaceId sid, NoiaPosition pos);

//------------------------------------------------------------------------------

/// Axis event object.
/// AxisData structure with reference counting.
typedef struct {
    NoiaObject base;
    NoiaAxisData axisdata;
} NoiaAxisObject;

NoiaAxisObject* noia_axis_create(double h, double v, int hd, int vd);

//------------------------------------------------------------------------------

/// Compositor action context.
typedef struct {
    NoiaObject base;
    NoiaAction action;
    NoiaDirection direction;
    char* str;
    int magnitude;
} NoiaCommand;

/// Create the command.
NoiaCommand* noia_command_create(void);

/// Destroy the command.
void noia_command_destroy(NoiaCommand* command);

/// Clear the command.
void noia_command_clean(NoiaCommand* command);

/// Copy the command.
NoiaCommand* noia_command_copy(NoiaCommand* command);

//------------------------------------------------------------------------------

#endif // NOIA_GLOBAL_OBJECTS_H

