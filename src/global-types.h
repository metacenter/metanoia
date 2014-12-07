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
struct AuraRenderer;
typedef struct {
    int (*initialize) (struct AuraRenderer*);
    void (*finalize) (struct AuraRenderer*);
    void (*attach) (struct AuraRenderer*, SurfaceId, void*);
    void (*draw) (struct AuraRenderer*, Chain*);
    void (*free) (struct AuraRenderer*);
} AuraRenderer;

// Key event data
typedef struct {
    unsigned time;
    int code;
    bool value;
} AuraKeyData;

#endif // __AURA_GLOBAL_TYPES_H__

