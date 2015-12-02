// file: keyboard-bindings.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-bindings.h"
#include "keyboard-mode.h"

#include "utils-log.h"
#include "utils-list.h"

#include <malloc.h>
#include <search.h>
#include <linux/input.h>

//------------------------------------------------------------------------------

static NoiaBindingContext sContext = NOIA_BINDING_CONTEXT_INIT;

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

void noia_keyboard_add_binding(NoiaModeEnum modeid, const NoiaBinding* binding)
{
    NoiaMode* mode = NULL;

    // Check binding validity
    if (binding == 0 || binding->execute == 0) {
        LOG_WARN1("Invalid binding!");
        return;
    }

    if (!sContext.modes) {
        sContext.modes = noia_list_new((NoiaFreeFunc) noia_mode_free);
        noia_action_clean(&sContext.action);
    }

    // Try to find mode
    FOR_EACH (sContext.modes, link) {
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
        noia_list_append(sContext.modes, mode);
    }

    // Add binding
    noia_mode_add_binding(mode, binding);
}

//------------------------------------------------------------------------------

void noia_keyboard_mode_make_active(NoiaModeEnum modeid, bool active)
{
    FOR_EACH (sContext.modes, link) {
        NoiaMode* mode = (NoiaMode*) link->data;
        if (mode->modeid == modeid) {
            mode->active = active;
            break;
        }
    }
}

//------------------------------------------------------------------------------

bool noia_keyboard_catch_key(int code, NoiaKeyState state)
{
    // Update modifiers
    switch (code) {
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            sContext.modifiers = state
                        ? sContext.modifiers |  NOIA_KEY_CTRL
                        : sContext.modifiers & ~NOIA_KEY_CTRL;
            return 0;

        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            sContext.modifiers = state
                        ? sContext.modifiers |  NOIA_KEY_SHIFT
                        : sContext.modifiers & ~NOIA_KEY_SHIFT;
            return 0;

        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            sContext.modifiers = state
                        ? sContext.modifiers |  NOIA_KEY_ALT
                        : sContext.modifiers & ~NOIA_KEY_ALT;
            return 0;

        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
            sContext.modifiers = state
                        ? sContext.modifiers |  NOIA_KEY_META
                        : sContext.modifiers & ~NOIA_KEY_META;
            return 0;
    }

    sContext.code = code;
    sContext.state = state;
    if (state == NOIA_KEY_RELEASED) {
        sContext.code = 0x0;
        return 0;
    }

    NoiaBinding* binding =
           noia_keyboard_find_binding(sContext.modes, code, sContext.modifiers);
    if (!binding) {
        return 0;
    }

    binding->execute(&sContext);
    return 1;
}

//------------------------------------------------------------------------------

void noia_keyboard_free_all(void)
{
    noia_list_free(sContext.modes);
}

//------------------------------------------------------------------------------

