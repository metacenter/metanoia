// file: constants.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CONSTANTS_H__
#define __AURA_CONSTANTS_H__

#include "global-types.h"

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
