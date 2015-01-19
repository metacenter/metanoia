// file: constants.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONSTANTS_H__
#define __AURA_CONSTANTS_H__

#include "global-types.h"

#define AURA_UNUSED __attribute__((unused))

typedef enum {
    AURA_KEY_RELEASED,
    AURA_KEY_PRESSED,
} AuraKeyState;

typedef enum {
    AURA_KEY_CTRL  = 0x0001,
    AURA_KEY_SHIFT = 0x0002,
    AURA_KEY_ALT   = 0x0004,
} AuraKeyModifierFlag;

typedef enum {
    AURA_NORMAL_MODE,
    AURA_MODE_NUM,
} AuraModeEnum;

// TBR
static const SurfaceId scInvalidSurfaceId = 0x0000;

/// Argmand types
typedef enum {
    AURA_ARGMAND_NONE,    ///< Dummy; do/parametrize nothing
    // actions
    AURA_ARGMAND_FOCUS,   ///< Change focused surface
    AURA_ARGMAND_MOVE,    ///< Change position
    AURA_ARGMAND_JUMP,    ///< Change palcement in tree structure
    AURA_ARGMAND_RESIZE,  ///< Change size
    // directions
    AURA_ARGMAND_N,       ///< North; up; above
    AURA_ARGMAND_E,       ///< East; right
    AURA_ARGMAND_S,       ///< South; down; below
    AURA_ARGMAND_W,       ///< West; left
    AURA_ARGMAND_BACK,    ///< Back in time; most recently used
    AURA_ARGMAND_FORWARD, ///< Forward in time; the oldest used
} AuraArgmandType;

/// Function return values or error codes
typedef enum {
    AURA_SUCCESS = 0,       ///< Everything worked fine
    AURA_ERROR,             ///< Unspecified error
    AURA_INCORECT_ARGUMENT, ///< Incorrect or invalid argument passed
} AuraResult;

#endif // __AURA_CONSTANTS_H__

