// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_GLOBAL_TYPES_H__
#define __AURA_GLOBAL_TYPES_H__

#include "utils/chain.h"

#include <stdint.h>

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
    void (*draw_surfaces) (struct AuraRenderer*, Chain*);
    void (*free) (struct AuraRenderer*);
} AuraRenderer;

// Screen output // TODO move to separate file
struct AuraOutput;
typedef struct {
    // TODO: keep list of available modes instead of just width and height
    int width;
    int height;
    AuraRenderer* (*initialize) (struct AuraOutput*, int, int);
} AuraOutput;

#endif // __AURA_GLOBAL_TYPES_H__

