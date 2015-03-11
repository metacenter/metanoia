// file: keyboard-mode.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_KEYBOARD_MODE_H__
#define __NOIA_KEYBOARD_MODE_H__

#include "keyboard-binding.h"
#include "global-constants.h"

typedef struct {
    NoiaModeEnum modeid;
    void* bindings;
    int active;
} NoiaMode;

NoiaMode* noia_mode_new(NoiaModeEnum modeid);

void noia_mode_free(NoiaMode* self);

void noia_mode_add_binding(NoiaMode* self, const NoiaBinding* binding);

NoiaBinding* noia_mode_find_binding(NoiaMode* self,
                                    int code,
                                    uint32_t Noiamodifiers);

#endif // __NOIA_KEYBOARD_MODE_H__

