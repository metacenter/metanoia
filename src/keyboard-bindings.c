// file: keyboard-bindings.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-bindings.h"
#include "utils-log.h"

#include <malloc.h>
#include <search.h>
#include <linux/input.h>

static void* binding_tree = 0;
static uint32_t modifiers = 0;

//------------------------------------------------------------------------------

static int compare(const Binding* binding1, const Binding* binding2)
{
    if (binding1->modifiers < binding2->modifiers) return -1;
    if (binding1->modifiers > binding2->modifiers) return  1;

    if (binding1->code < binding2->code) return -1;
    if (binding1->code > binding2->code) return  1;

    return 0;
}

//------------------------------------------------------------------------------

void aura_keyboard_add_binding(const Binding* binding)
{
    if (binding == 0 || binding->callback == 0) {
        LOG_WARN1("Invalid binding!");
        return;
    }

    // Skip if already exists
    if (tfind((void *) binding, &binding_tree, (CompareFunc) compare) != NULL) {
        LOG_WARN2("Binding already exists! (code: %d, modifiers: %d)",
                  binding->code, binding->modifiers);
        return;
    }

    // Add binding
    Binding* bind = malloc(sizeof(Binding));
    *bind = *binding;

    if (tsearch((void *) bind, &binding_tree, (CompareFunc) compare) == NULL) {
        LOG_ERROR("Could not store binding!");
        free(bind);
        return;
    }

    LOG_DATA2("Created binding (code: %d, modifiers: %d)",
              binding->code, binding->modifiers);
}

//------------------------------------------------------------------------------

void aura_keyboard_remove_binding(uint32_t code,
                                  uint32_t modifiers)
{
    // TODO
}

//------------------------------------------------------------------------------

int aura_keyboard_catch_key(int code, KeyState state)
{
    Binding searched;
    Binding** found;

    // Update modifiers
    switch (code) {
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            modifiers = state
                        ? modifiers |  AURA_KEY_CTRL
                        : modifiers & ~AURA_KEY_CTRL;
            return -2;

        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            modifiers = state
                        ? modifiers |  AURA_KEY_SHIFT
                        : modifiers & ~AURA_KEY_SHIFT;
            return -2;

        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            modifiers = state
                        ? modifiers |  AURA_KEY_ALT
                        : modifiers & ~AURA_KEY_ALT;
            return -2;
    }

    // Search for binding
    searched.code = code;
    searched.modifiers = modifiers;
    found = tfind((void*) &searched, &binding_tree, (CompareFunc) compare);

    // If not found return negative...
    if (found == 0 || *found == 0 || (*found)->callback == 0) {
        return -1;
    }

    // otherwise positive.
    (*found)->callback();
    return 1;
}

//------------------------------------------------------------------------------

