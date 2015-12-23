// file: global-enums.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_GLOBAL_ENUMS_H
#define NOIA_GLOBAL_ENUMS_H

#include <stdbool.h>

/// Enumerate key states
typedef enum {
    NOIA_KEY_RELEASED,
    NOIA_KEY_PRESSED,
} NoiaKeyState;

/// Enumerate all used modifiers
typedef enum {
    NOIA_KEY_NONE  = 0x0000,
    NOIA_KEY_CTRL  = 0x0001,
    NOIA_KEY_SHIFT = 0x0002,
    NOIA_KEY_ALT   = 0x0004,
    NOIA_KEY_META  = 0x0008,
} NoiaKeyModifierFlag;

/// Enumerate key binding modes.
/// The mode is set of key bindings that can be active.
typedef enum {
    NOIA_MODE_COMMON,
    NOIA_MODE_NORMAL,
    NOIA_MODE_INSERT,
    NOIA_MODE_NUM, ///< Guard
} NoiaModeEnum;

/// Argmand types.
/// Argmand is a data type that can be a command or
/// an argument (parameter) depending on context.
typedef enum {
    NOIA_ARGMAND_NONE = 0,  ///< Dummy; do/parametrize nothing
    NOIA_ARGMAND_NUMBER,    ///< Number; numeric value; integer
    NOIA_ARGMAND_ANCHOR,    ///< Anchorize; de-anchorize
    NOIA_ARGMAND_CONF,      ///< Change configuration
    NOIA_ARGMAND_FOCUS,     ///< Change focus
    NOIA_ARGMAND_SWAP,      ///< Swap
    NOIA_ARGMAND_MOVE,      ///< Change position
    NOIA_ARGMAND_JUMP,      ///< Change placement in tree structure
    NOIA_ARGMAND_RESIZE,    ///< Change size
    NOIA_ARGMAND_N,         ///< North; up; above
    NOIA_ARGMAND_E,         ///< East; right
    NOIA_ARGMAND_S,         ///< South; down; below
    NOIA_ARGMAND_W,         ///< West; left
    NOIA_ARGMAND_BACK,      ///< Back in time; most recently used
    NOIA_ARGMAND_FORWARD,   ///< Forward in time; the oldest used
    NOIA_ARGMAND_BEGIN,     ///< Begin; start; head
    NOIA_ARGMAND_END,       ///< End; finish; tail
    NOIA_ARGMAND_TRUNK,     ///< Trunk; make a trunk; up in frame hierarchy
    NOIA_ARGMAND_WORKSPACE, ///< Workspace
} NoiaArgmand;

/// Frame types.
typedef enum {
    NOIA_FRAME_TYPE_NONE       = 0x0000,
    NOIA_FRAME_TYPE_STACKED    = 0x0001,
    NOIA_FRAME_TYPE_HORIZONTAL = 0x0002,
    NOIA_FRAME_TYPE_VERTICAL   = 0x0004,
    NOIA_FRAME_TYPE_FLOATING   = 0x0008,
    NOIA_FRAME_TYPE_FIXED      = 0x0010,
    NOIA_FRAME_TYPE_LEAF       = 0x0020,

    NOIA_FRAME_TYPE_SPECIAL    = 0x1000,

    NOIA_FRAME_TYPE_DIRECTED   = NOIA_FRAME_TYPE_HORIZONTAL
                               | NOIA_FRAME_TYPE_VERTICAL
                               | NOIA_FRAME_TYPE_STACKED,

    NOIA_FRAME_TYPE_WORKSPACE  = NOIA_FRAME_TYPE_SPECIAL
                               | NOIA_FRAME_TYPE_FIXED
                               | NOIA_FRAME_TYPE_STACKED,
} NoiaFrameType;

/// Type of transformation used for background image.
typedef enum {
    NOIA_BG_TRANS_REPEAT,
    NOIA_BG_TRANS_CENTER,
    NOIA_BG_TRANS_SCALE,
    NOIA_BG_TRANS_STRETCH,
} NoiaBGTransform;

/// Function return values or error codes
typedef enum {
    NOIA_RESULT_SUCCESS = 0,        ///< Everything worked fine
    NOIA_RESULT_ERROR,              ///< Unspecified error
    NOIA_RESULT_INCORRECT_ARGUMENT, ///< Incorrect or invalid argument passed
    NOIA_RESULT_NOT_FOUND,          ///< Required data not found
} NoiaResult;

/// Asserts if the result is `NOIA_RESULT_SUCCESS`.
/// @see NoiaResult NOIA_TRY
#define NOIA_ASSERT_RESULT(RESULT) { if (RESULT != NOIA_RESULT_SUCCESS) break; }

/// Check if given argmand can define direction.
/// @see NoiaArgmand
bool noia_argmand_is_directed(NoiaArgmand argmand);

/// Check if given argmand can define an action.
/// @see NoiaArgmand
bool noia_argmand_is_actionable(NoiaArgmand argmand);

/// If given argmand is directed, return and argmand pointing in opposite
/// direction.
/// @see noia_argmand_is_directed NoiaArgmand
NoiaArgmand noia_argmand_reverse_directed(NoiaArgmand argmand);

#endif // NOIA_GLOBAL_ENUMS_H

