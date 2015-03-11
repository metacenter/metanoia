// file: global-objects.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_GLOBAL_OBJECTS_H__
#define __NOIA_GLOBAL_OBJECTS_H__

#include "global-types.h"
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

NoiaKeyObject* noia_key_create(unsigned time, int code, bool value);

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

#endif // __NOIA_GLOBAL_OBJECTS_H__

