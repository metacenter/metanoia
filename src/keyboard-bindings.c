// file: keyboard-bindings.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-bindings.h"
#include "keyboard-mode.h"
#include "keyboard-argmand.h"

#include "utils-log.h"

#include <malloc.h>
#include <search.h>
#include <linux/input.h>

//------------------------------------------------------------------------------

static NoiaList* stack = NULL;
static NoiaList* modes = NULL;
static uint32_t modifiers = 0;

//------------------------------------------------------------------------------

void noia_keyboard_add_binding(NoiaModeEnum modeid, const NoiaBinding* binding)
{
    NoiaMode* mode = NULL;

    // Check binding validity
    if (binding == 0 || binding->execute == 0) {
        LOG_WARN1("Invalid binding!");
        return;
    }

    if (!stack) {
        stack = noia_list_new((NoiaFreeFunc) noia_argmand_free);
    }

    if (!modes) {
        modes = noia_list_new((NoiaFreeFunc) noia_mode_free);
    }

    // Try to find mode
    FOR_EACH (modes, link) {
        mode = (NoiaMode*) link->data;
        if (mode->modeid != modeid) {
            mode = NULL;
        } else {
            break;
        }
    }

    // If mode not found - create it
    if (!mode) {
        mode = noia_mode_new(modeid);
        noia_list_append(modes, mode);
        if (modeid == NOIA_NORMAL_MODE) {
            mode->active = 1;
        }
    }

    // Add binding
    noia_mode_add_binding(mode, binding);
}

//------------------------------------------------------------------------------

void noia_keyboard_free_all()
{
    noia_list_free(modes);
    noia_list_free(stack);
}

//------------------------------------------------------------------------------

NoiaBinding* noia_keyboard_find_binding(NoiaList* modes,
                                        int code,
                                        uint32_t modifiers)
{
    NoiaBinding* binding = NULL;

    FOR_EACH (modes, link) {
        NoiaMode* mode = (NoiaMode*) link->data;
        if (!mode->active) {
            continue;
        }

        binding = noia_mode_find_binding(mode, code, modifiers);
        if (binding) {
            break;
        }
    }

    return binding;
}

//------------------------------------------------------------------------------

bool noia_keyboard_catch_key(int code, NoiaKeyState state)
{
    // Update modifiers
    switch (code) {
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            modifiers = state
                        ? modifiers |  NOIA_KEY_CTRL
                        : modifiers & ~NOIA_KEY_CTRL;
            return 0;

        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            modifiers = state
                        ? modifiers |  NOIA_KEY_SHIFT
                        : modifiers & ~NOIA_KEY_SHIFT;
            return 0;

        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            modifiers = state
                        ? modifiers |  NOIA_KEY_ALT
                        : modifiers & ~NOIA_KEY_ALT;
            return 0;

        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
            modifiers = state
                        ? modifiers |  NOIA_KEY_META
                        : modifiers & ~NOIA_KEY_META;
            return 0;
    }

    if (state == NOIA_KEY_RELEASED) {
        return 0;
    }

    NoiaBinding* binding = noia_keyboard_find_binding(modes, code, modifiers);
    if (!binding) {
        return 0;
    }

    binding->execute(stack, code, modifiers, state);
    return 1;
}

//------------------------------------------------------------------------------

