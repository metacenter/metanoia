// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_GLOBAL_TYPES_H__
#define __AURA_GLOBAL_TYPES_H__

#include "utils-chain.h"

#include <stdint.h>
#include <stdbool.h>

// TBR
typedef uint32_t SurfaceId;

typedef void (*AuraKeyCallback) (void);
typedef int (*CompareFunc) (const void*, const void*);

// Keyboard binding
typedef struct {
    int code;
    uint32_t modifiers;
    AuraKeyCallback callback;
} Binding;

// Renderer strategy interface // TODO: move to separate file
// TODO: move 'attach' elsewhere
typedef struct AuraRenderer AuraRenderer;
struct AuraRenderer {
    int (*initialize) (AuraRenderer*);
    void (*finalize) (AuraRenderer*);
    void (*attach) (AuraRenderer*, SurfaceId, void*);
    void (*draw) (AuraRenderer*, Chain*, int x, int y);
    void (*free) (AuraRenderer*);
};

// Key event data
typedef struct {
    unsigned time;
    int code;
    bool value;
} AuraKeyData;

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

#endif // __AURA_GLOBAL_TYPES_H__

