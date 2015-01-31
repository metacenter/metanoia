// file: keyboard-bindings.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-bindings.h"
#include "keyboard-mode.h"

#include "utils-log.h"

#include <malloc.h>
#include <search.h>
#include <linux/input.h>

//------------------------------------------------------------------------------

static Chain* stack = NULL;
static Chain* modes = NULL;
static uint32_t modifiers = 0;

//------------------------------------------------------------------------------

void aura_keyboard_add_binding(AuraModeEnum modeid, const AuraBinding* binding)
{
    AuraMode* mode = NULL;

    // Check binding validity
    if (binding == 0 || binding->execute == 0) {
        LOG_WARN1("Invalid binding!");
        return;
    }

    if (!stack) {
        stack = chain_new(NULL);
    }

    if (!modes) {
        modes = chain_new((AuraFreeFunc) aura_mode_free);
    }

    // Try to find mode
    for (Link* link = modes->first; link; link = link->next) {
        mode = (AuraMode*) link->data;
        if (mode->modeid != modeid) {
            mode = NULL;
        } else {
            break;
        }
    }

    // If mode not found - create it
    if (!mode) {
        mode = aura_mode_new(modeid);
        chain_append(modes, mode);
        if (modeid == AURA_NORMAL_MODE) {
            mode->active = 1;
        }
    }

    // Add binding
    aura_mode_add_binding(mode, binding);
}

//------------------------------------------------------------------------------

void aura_keyboard_free_all()
{
    chain_free(modes);
    chain_free(stack);
}

//------------------------------------------------------------------------------

AuraBinding* aura_keyboard_find_binding(Chain* modes,
                                        int code,
                                        uint32_t modifiers)
{
    AuraBinding* binding = NULL;

    Link* link;
    for (link = modes->first; link; link = link->next) {
        AuraMode* mode = (AuraMode*) link->data;
        if (!mode->active) {
            continue;
        }

        binding = aura_mode_find_binding(mode, code, modifiers);
        if (binding) {
            break;
        }
    }

    return binding;
}

//------------------------------------------------------------------------------

bool aura_keyboard_catch_key(int code, AuraKeyState state)
{
    // Update modifiers
    switch (code) {
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            modifiers = state
                        ? modifiers |  AURA_KEY_CTRL
                        : modifiers & ~AURA_KEY_CTRL;
            return 0;

        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            modifiers = state
                        ? modifiers |  AURA_KEY_SHIFT
                        : modifiers & ~AURA_KEY_SHIFT;
            return 0;

        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            modifiers = state
                        ? modifiers |  AURA_KEY_ALT
                        : modifiers & ~AURA_KEY_ALT;
            return 0;
    }

    if (state == AURA_KEY_RELEASED) {
        return 0;
    }

    AuraBinding* binding = aura_keyboard_find_binding(modes, code, modifiers);
    if (!binding) {
        return 0;
    }

    binding->execute(stack, code, modifiers, state);
    return 1;
}

//------------------------------------------------------------------------------

