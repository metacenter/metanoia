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
    AURA_ARGMAND_NONE,
    // actions
    AURA_ARGMAND_FOCUS,
    AURA_ARGMAND_MOVE,
    AURA_ARGMAND_RESIZE,
    // directions
    AURA_ARGMAND_N, ///< North
    AURA_ARGMAND_E, ///< East
    AURA_ARGMAND_S, ///< South
    AURA_ARGMAND_W, ///< West
    AURA_ARGMAND_BACK,
    AURA_ARGMAND_FORWARD,
} AuraArgmandType;

#endif // __AURA_GLOBAL_TYPES_H__

