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

void aura_keyboard_add_argmand(AuraModeEnum modeid, const AuraArgmand* argmand)
{
    AuraMode* mode = NULL;

    // Check argmand validity
    if (argmand == 0 || argmand->execute == 0) {
        LOG_WARN1("Invalid argmand!");
        return;
    }

    if (!stack) {
        stack = chain_new(0);
    }

    if (!modes) {
        modes = chain_new(0);
    }

    // Try to find mode
    Link* link;
    for (link = modes->first; link; link = link->next) {
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

    // Add argmand
    aura_mode_add_argmand(mode, argmand);
}

//------------------------------------------------------------------------------

AuraArgmand* aura_keyboard_find_argmand(Chain* modes,
                                        int code,
                                        uint32_t modifiers)
{
    AuraArgmand* argmand = NULL;

    Link* link;
    for (link = modes->first; link; link = link->next) {
        AuraMode* mode = (AuraMode*) link->data;
        if (!mode->active) {
            continue;
        }

        argmand = aura_mode_find_argmand(mode, code, modifiers);
        if (argmand) {
            break;
        }
    }

    return argmand;
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

    AuraArgmand* argmand = aura_keyboard_find_argmand(modes, code, modifiers);
    if (!argmand) {
        return 0;
    }

    argmand->execute(stack);
    return 1;
}

//------------------------------------------------------------------------------

