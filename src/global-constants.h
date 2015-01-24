// file: constants.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONSTANTS_H__
#define __AURA_CONSTANTS_H__

#include "global-types.h"

#define AURA_UNUSED __attribute__((unused))

/// Invalid value for AuraItemId
static const AuraItemId scInvalidItemId = 0;

/// Invalid value for AuraSurfaceId
static const AuraSurfaceId scInvalidSurfaceId = 0;

/// Enumerate key states
typedef enum {
    AURA_KEY_RELEASED,
    AURA_KEY_PRESSED,
} AuraKeyState;

/// Enumerate all used modifiers
typedef enum {
    AURA_KEY_CTRL  = 0x0001,
    AURA_KEY_SHIFT = 0x0002,
    AURA_KEY_ALT   = 0x0004,
} AuraKeyModifierFlag;

/// Enumerate key binding modes.
/// The mode is set of key bindings that can be active.
typedef enum {
    AURA_NORMAL_MODE,
    AURA_MODE_NUM, ///< Guard
} AuraModeEnum;

/// Argmand types.
/// Argmand is a data type that can be a command or
/// an argument (parameter) depending on context.
typedef enum {
    AURA_ARGMAND_NONE,    ///< Dummy; do/parametrize nothing
    // values
    AURA_ARGMAND_NUMBER,  ///< Number; numeric value; integer
    AURA_ARGMAND_STRING,  ///< String
    // actions
    AURA_ARGMAND_FOCUS,   ///< Change focused surface
    AURA_ARGMAND_MOVE,    ///< Change position
    AURA_ARGMAND_JUMP,    ///< Change placement in tree structure
    AURA_ARGMAND_RESIZE,  ///< Change size
    // directions
    AURA_ARGMAND_N,       ///< North; up; above
    AURA_ARGMAND_E,       ///< East; right
    AURA_ARGMAND_S,       ///< South; down; below
    AURA_ARGMAND_W,       ///< West; left
    AURA_ARGMAND_BACK,    ///< Back in time; most recently used
    AURA_ARGMAND_FORWARD, ///< Forward in time; the oldest used
    AURA_ARGMAND_BEGIN,   ///< Begin; start; head
    AURA_ARGMAND_END,     ///< End; finnish; tail
} AuraArgmandType;

/// Function return values or error codes
typedef enum {
    AURA_RESULT_SUCCESS = 0,        ///< Everything worked fine
    AURA_RESULT_ERROR,              ///< Unspecified error
    AURA_RESULT_INCORRECT_ARGUMENT, ///< Incorrect or invalid argument passed
    AURA_RESULT_NOT_FOUND,          ///< Required data not found
} AuraResult;

#endif // __AURA_CONSTANTS_H__

