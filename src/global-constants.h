// file: constants.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_CONSTANTS_H__
#define __NOIA_CONSTANTS_H__

#include "global-types.h"

/// Invalid value for NoiaItemId
static const NoiaItemId scInvalidItemId = 0;

/// Invalid value for NoiaSurfaceId
static const NoiaSurfaceId scInvalidSurfaceId = 0;

/// Enumerate key states
typedef enum {
    NOIA_KEY_RELEASED,
    NOIA_KEY_PRESSED,
} NoiaKeyState;

/// Enumerate all used modifiers
typedef enum {
    NOIA_KEY_NONE  = 0x0000,
    NOIA_KEY_CTRL  = 0x0001,
    NOIA_KEY_SHIFT = 0x0002,
    NOIA_KEY_ALT   = 0x0004,
    NOIA_KEY_META  = 0x0008,
} NoiaKeyModifierFlag;

/// Enumerate key binding modes.
/// The mode is set of key bindings that can be active.
typedef enum {
    NOIA_NORMAL_MODE,
    NOIA_MODE_NUM, ///< Guard
} NoiaModeEnum;

/// Argmand types.
/// Argmand is a data type that can be a command or
/// an argument (parameter) depending on context.
typedef enum {
    NOIA_ARGMAND_NONE,    ///< Dummy; do/parametrize nothing
    // values
    NOIA_ARGMAND_NUMBER,  ///< Number; numeric value; integer
    NOIA_ARGMAND_STRING,  ///< String
    // actions
    NOIA_ARGMAND_FOCUS,   ///< Change focused surface
    NOIA_ARGMAND_MOVE,    ///< Change position
    NOIA_ARGMAND_JUMP,    ///< Change placement in tree structure
    NOIA_ARGMAND_RESIZE,  ///< Change size
    // directions
    NOIA_ARGMAND_N,       ///< North; up; above
    NOIA_ARGMAND_E,       ///< East; right
    NOIA_ARGMAND_S,       ///< South; down; below
    NOIA_ARGMAND_W,       ///< West; left
    NOIA_ARGMAND_BACK,    ///< Back in time; most recently used
    NOIA_ARGMAND_FORWARD, ///< Forward in time; the oldest used
    NOIA_ARGMAND_BEGIN,   ///< Begin; start; head
    NOIA_ARGMAND_END,     ///< End; finnish; tail
} NoiaArgmandType;

/// Function return values or error codes
typedef enum {
    NOIA_RESULT_SUCCESS = 0,        ///< Everything worked fine
    NOIA_RESULT_ERROR,              ///< Unspecified error
    NOIA_RESULT_INCORRECT_ARGUMENT, ///< Incorrect or invalid argument passed
    NOIA_RESULT_NOT_FOUND,          ///< Required data not found
} NoiaResult;

#define NOIA_ASSERT_RESULT(RESULT) { if (RESULT != NOIA_RESULT_SUCCESS) break; }

#endif // __NOIA_CONSTANTS_H__

