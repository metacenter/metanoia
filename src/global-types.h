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

// Type defining position or point coordinates
typedef struct {
    int x;
    int y;
} AuraPosition;

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

#endif // __AURA_GLOBAL_TYPES_H__

