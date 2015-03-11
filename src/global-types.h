// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_GLOBAL_TYPES_H__
#define __NOIA_GLOBAL_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

/// Id for NoiaItem
typedef uintptr_t NoiaItemId;

/// Free function definition
typedef void (*NoiaFreeFunc) (void*);

/// Compare function definition
typedef int (*NoiaCompareFunc) (void*, void*);

/// Structure to be inherited by all types that want to be stored in NoiaStore
typedef struct {
    union {
        NoiaItemId id;
        char* str;
    };
} NoiaItem;

/// Identifier of a surface
typedef NoiaItemId NoiaSurfaceId;

/// Callback used in NoiaBinding structure
typedef void (*NoiaKeyCallback) (void);

/// Type defining position, point coordinates or 2D vector
typedef struct {
    int x;
    int y;
} NoiaPosition;

/// Type defining 2D size, dimensions or resolution
typedef struct {
    int width;
    int height;
} NoiaSize;

/// Key event data
typedef struct {
    unsigned time;
    int code;
    bool value;
} NoiaKeyData;

/// Button event data
typedef struct {
    unsigned time;
    int code;
    bool value;
} NoiaButtonData;

#endif // __NOIA_GLOBAL_TYPES_H__

