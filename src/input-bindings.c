// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "input-bindings.h"
#include "input-mode.h"

#include "utils-log.h"
#include "utils-list.h"

#include <malloc.h>
#include <search.h>
#include <linux/input.h>

//------------------------------------------------------------------------------

NoiaBinding* noia_input_find_binding(NoiaList* modes,
                                     int code,
                                     uint32_t modifiers)
{
    NoiaBinding* binding = NULL;

    FOR_EACH (modes, link) {
        NoiaMode* mode = (NoiaMode*) link->data;
        if (not mode->active) {
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

void noia_input_add_binding(NoiaList* modes,
                            NoiaModeEnum modeid,
                            const NoiaBinding* binding)
{
    NoiaMode* mode = NULL;

    // Check binding validity
    NOIA_ENSURE(binding, return);
    NOIA_ENSURE(binding->execute, return);

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
    if (not mode) {
        mode = noia_mode_create(modeid);
        noia_list_append(modes, mode);
    }

    // Add binding
    noia_mode_add_binding(mode, binding);
}

//------------------------------------------------------------------------------

void noia_input_mode_make_active(NoiaList* modes,
                                 NoiaModeEnum modeid,
                                 bool active)
{
    FOR_EACH (modes, link) {
        NoiaMode* mode = (NoiaMode*) link->data;
        if (mode->modeid == modeid) {
            mode->active = active;
            break;
        }
    }
}

//------------------------------------------------------------------------------

bool noia_input_catch_key(NoiaList* modes,
                          NoiaInputContext* context,
                          int code,
                          NoiaKeyState state)
{
    // Update modifiers
    switch (code) {
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            context->modifiers = state
                               ? context->modifiers |  NOIA_KEY_CTRL
                               : context->modifiers & ~NOIA_KEY_CTRL;
            return false;

        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            context->modifiers = state
                               ? context->modifiers |  NOIA_KEY_SHIFT
                               : context->modifiers & ~NOIA_KEY_SHIFT;
            return false;

        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            context->modifiers = state
                               ? context->modifiers |  NOIA_KEY_ALT
                               : context->modifiers & ~NOIA_KEY_ALT;
            return false;

        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
            context->modifiers = state
                               ? context->modifiers |  NOIA_KEY_META
                               : context->modifiers & ~NOIA_KEY_META;
            return false;
    }

    context->code = code;
    context->state = state;
    if (state == NOIA_KEY_RELEASED) {
        context->code = 0x0;
        return false;
    }

    NoiaBinding* binding =
                       noia_input_find_binding(modes, code, context->modifiers);
    if (not binding) {
        return false;
    }

    binding->execute(context);
    return true;
}

//------------------------------------------------------------------------------

