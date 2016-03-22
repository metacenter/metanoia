// file: global-objects.h
// vim: tabstop=4 expandtab colorcolumn=81 list

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

/// Compositor action context.
typedef struct {
    NoiaObject base;
    NoiaArgmand action;
    NoiaArgmand direction;
    char* str;
    int magnitude;
} NoiaAction;

/// Create the action.
NoiaAction* noia_action_create(void);

/// Destroy the action.
void noia_action_destroy(NoiaAction* action);

/// Clear the action.
void noia_action_clean(NoiaAction* action);

/// Copy the action.
NoiaAction* noia_action_copy(NoiaAction* action);

//------------------------------------------------------------------------------

#endif // NOIA_GLOBAL_OBJECTS_H

