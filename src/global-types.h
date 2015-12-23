// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_GLOBAL_TYPES_H
#define NOIA_GLOBAL_TYPES_H

#include <stdint.h>
#include <limits.h>

#include "global-enums.h"

/// Id for NoiaItem
typedef uintptr_t NoiaItemId;

/// Free function definition
typedef void (*NoiaFreeFunc) (void*);

/// Compare function definition
typedef int (*NoiaCompareFunc) (const void*, const void*);

/// Duplicate function definition
typedef void* (*NoiaDuplicateFunc) (void*);

/// Print formated string function
typedef int (*NoiaPrintFunc) (const char*, ...);

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

/// Container for color data.
typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} NoiaColor;

/// Container for all data required to draw an image.
typedef struct {
    int width;
    int height;
    int stride;
    uint8_t* data;
} NoiaBuffer;

/// Data needed by Renderer to draw surface
typedef struct {
    NoiaSurfaceId sid;
    NoiaPosition pos;
} NoiaSurfaceContext;

/// Data needed by Renderer to draw layout
typedef struct {
    NoiaSurfaceContext pointer;
} NoiaLayoverContext;

/// Compositor action context
typedef struct {
    NoiaArgmand action;
    NoiaArgmand direction;
    char* str;
    int magnitude;
} NoiaAction;

/// Check if point `position` is inside area `area`.
bool noia_position_is_inside(NoiaPosition position, NoiaArea area);

/// If point `position` is outside area `area` return a point inside area `area`
/// that is the closest to point `position`.
NoiaPosition noia_position_cast(NoiaPosition position, NoiaArea area);

/// Clear the buffer.
void noia_buffer_clean(NoiaBuffer* buffer);

/// Clear the action.
void noia_action_clean(NoiaAction* action);

#endif // NOIA_GLOBAL_TYPES_H

