// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_GLOBAL_TYPES_H__
#define __NOIA_GLOBAL_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

/// @todo Move to global-macros.h
#include <assert.h>
#define NOIA_TRY switch(0) default:

/// Id for NoiaItem
typedef uintptr_t NoiaItemId;

/// Free function definition
typedef void (*NoiaFreeFunc) (void*);

/// Compare function definition
typedef int (*NoiaCompareFunc) (void*, void*);

/// Duplicate function definition
typedef void* (*NoiaDuplicateFunc) (void*);

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

/// Type defining 2D area
typedef struct {
    NoiaPosition pos;
    NoiaSize size;
} NoiaArea;

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

/// Data needed by Renderer to draw surface
typedef struct {
    NoiaSurfaceId sid;
    NoiaPosition pos;
} NoiaSurfaceContext;

/// Data needed by Renderer to draw layover
typedef struct NoiaLayoverContextStruct {
    NoiaSurfaceContext pointer;
} NoiaLayoverContext;

bool noia_position_is_inside(NoiaPosition pos, NoiaArea area);
NoiaPosition noia_position_cast(NoiaPosition position, NoiaArea area);

#endif // __NOIA_GLOBAL_TYPES_H__

