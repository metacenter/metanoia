// file: keyboard-mode.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_KEYBOARD_MODE_H
#define NOIA_KEYBOARD_MODE_H

#include "keyboard-binding.h"
#include "global-constants.h"

/// Data type representing keyboard mode.
typedef struct {
    NoiaModeEnum modeid;
    void* bindings;
    bool active;
} NoiaMode;

/// Construct NoiaMode.
NoiaMode* noia_mode_new(NoiaModeEnum modeid);

/// Destruct NoiaMode.
void noia_mode_free(NoiaMode* self);

/// Add new keyboard binding to given mode.
void noia_mode_add_binding(NoiaMode* self, const NoiaBinding* binding);

/// Find binding specified by `code` and `modifiers` in given mode.
/// @return Pointer to binding if found; `NULL` otherwise.
NoiaBinding* noia_mode_find_binding(NoiaMode* self,
                                    int code,
                                    uint32_t Noiamodifiers);

#endif // NOIA_KEYBOARD_MODE_H

