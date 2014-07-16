// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_GLOBAL_TYPES_H__
#define __AURA_GLOBAL_TYPES_H__

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

// Renderer strategy interface
struct Renderer;
typedef struct {
    int (*initialize) (void);
    void (*finalize) (void);
    void (*free) (struct Renderer*);
} Renderer;

// Screen output
typedef struct {
    short width;
    short height;
    short stride;
    Renderer* renderer;
} Output;

#endif // __AURA_GLOBAL_TYPES_H__

