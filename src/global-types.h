// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_GLOBAL_TYPES_H__
#define __AURA_GLOBAL_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

// TBR
typedef uint32_t SurfaceId;

typedef void (*AuraKeyCallback) (void);
typedef int (*CompareFunc) (const void*, const void*);

// Type defining position, point coordinates or 2D vector
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

// Key event data
typedef struct {
    unsigned time;
    int code;
    bool value;
} AuraKeyData;

#endif // __AURA_GLOBAL_TYPES_H__

