// file: global-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_GLOBAL_TYPES_H__
#define __AURA_GLOBAL_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

/// Id for AuraItem
typedef uintptr_t AuraItemId;

/// Structure to be inherited by all types that want to be stored in AuraStore
typedef struct {
    union {
        AuraItemId id;
        char* str;
    };
} AuraItem;

/// Identifier of a surface
typedef AuraItemId AuraSurfaceId;

/// Callback used in AuraBinding structure
typedef void (*AuraKeyCallback) (void);

/// Type defining position, point coordinates or 2D vector
typedef struct {
    int x;
    int y;
} AuraPosition;

/// Type defining 2D size, dimensions or resolution
typedef struct {
    int width;
    int height;
} AuraSize;

/// Key event data
typedef struct {
    unsigned time;
    int code;
    bool value;
} AuraKeyData;

#endif // __AURA_GLOBAL_TYPES_H__

