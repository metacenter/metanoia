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
struct Renderer;
typedef struct {
    int (*initialize) (void);
    void (*finalize) (void);
    void (*draw_surfaces) (struct Renderer*, Chain*);
    void (*free) (struct Renderer*);
} Renderer;

// Screen output
typedef struct {
    short width;
    short height;
    Renderer* renderer;
} Output;

#endif // __AURA_GLOBAL_TYPES_H__

