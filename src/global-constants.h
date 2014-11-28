// file: constants.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONSTANTS_H__
#define __AURA_CONSTANTS_H__

#include "global-types.h"

typedef enum {
    LEVEL_ERROR,
    LEVEL_NYIMP,
    LEVEL_WARN1,
    LEVEL_DATA1,
    LEVEL_INFO1,
    LEVEL_WARN2,
    LEVEL_DATA2,
    LEVEL_INFO2,
    LEVEL_DEBUG,
    LEVEL_WARN3,
    LEVEL_DATA3,
    LEVEL_INFO3,
    LEVEL_WARN4,
    LEVEL_DATA4,
    LEVEL_INFO4,
} LogLevel;

typedef enum {
    AURA_KEY_RELEASED,
    AURA_KEY_PRESSED,
} KeyState;

typedef enum {
    AURA_KEY_CTRL  = 0x0001,
    AURA_KEY_SHIFT = 0x0002,
    AURA_KEY_ALT   = 0x0004,
} KeyModifierFlag;

// TBR
static const SurfaceId scInvalidSurfaceId = 0xFFFF;

#endif // __AURA_CONSTANTS_H__
