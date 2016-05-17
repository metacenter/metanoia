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

NoiaBindingExecuteFunc noia_input_find_binding(NoiaList* modes,
                                               int code,
                                               uint32_t modifiers)
{
    NoiaBindingExecuteFunc execute = NULL;

    FOR_EACH (modes, link) {
        NoiaMode* mode = (NoiaMode*) link->data;
        if (not mode->active) {
            continue;
        }

        execute = noia_mode_find_binding(mode, code, modifiers);
        if (execute) {
            break;
        }
    }

    return execute;
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
    bool result = false;

    // Update modifiers
    switch (code) {
    case KEY_LEFTCTRL:
    case KEY_RIGHTCTRL:
        context->modifiers = state
                           ? context->modifiers |  NOIA_KEY_CTRL
                           : context->modifiers & ~NOIA_KEY_CTRL;
        break;

    case KEY_LEFTSHIFT:
    case KEY_RIGHTSHIFT:
        context->modifiers = state
                           ? context->modifiers |  NOIA_KEY_SHIFT
                           : context->modifiers & ~NOIA_KEY_SHIFT;
        break;

    case KEY_LEFTALT:
    case KEY_RIGHTALT:
        context->modifiers = state
                           ? context->modifiers |  NOIA_KEY_ALT
                           : context->modifiers & ~NOIA_KEY_ALT;
        break;

    case KEY_LEFTMETA:
    case KEY_RIGHTMETA:
        context->modifiers = state
                           ? context->modifiers |  NOIA_KEY_META
                           : context->modifiers & ~NOIA_KEY_META;
        break;

    default:
        context->code = code;
        context->state = state;
        if (state == NOIA_KEY_RELEASED) {
            context->code = 0x0;
            break;
        }

        NoiaBindingExecuteFunc execute =
                       noia_input_find_binding(modes, code, context->modifiers);
        if (execute) {
            result = execute(context);
            break;
        }
    }
    return result;
}

//------------------------------------------------------------------------------

