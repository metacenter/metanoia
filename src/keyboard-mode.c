// file: keyboard-mode.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-mode.h"
#include "keyboard-binding.h"

#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

#define __USE_GNU
#define _GNU_SOURCE
#include <search.h>

//------------------------------------------------------------------------------

static int compare(const void* data1, const void* data2)
{
    const NoiaBinding* binding1 = (const NoiaBinding*) data1;
    const NoiaBinding* binding2 = (const NoiaBinding*) data2;

    if (binding1->modifiers < binding2->modifiers) return -1;
    if (binding1->modifiers > binding2->modifiers) return  1;

    if (binding1->code < binding2->code) return -1;
    if (binding1->code > binding2->code) return  1;

    return 0;
}

//------------------------------------------------------------------------------

NoiaMode* noia_mode_new(NoiaModeEnum modeid)
{
    NoiaMode* self = malloc(sizeof(NoiaMode));
    if (!self) {
        LOG_ERROR("Memory allocation failure!");
        return self;
    }

    self->modeid = modeid;
    self->bindings = NULL;
    self->active = 0;
    return self;
}

//------------------------------------------------------------------------------

void noia_mode_free(NoiaMode* self)
{
    if (!self) {
        return;
    }

    tdestroy(self->bindings, (NoiaFreeFunc) noia_binding_free);
    memset(self, 0, sizeof(NoiaMode));
    free(self);
}

//------------------------------------------------------------------------------

void noia_mode_add_binding(NoiaMode* self, const NoiaBinding* binding)
{
    void* found;

    // Skip if already exists
    found = tfind((void*) binding, &self->bindings, compare);
    if (found) {
        LOG_WARN2("Argmand already exists! (code: %d, modifiers: %d)",
                  binding->code, binding->modifiers);
        return;
    }

    // Add binding
    NoiaBinding* copy = noia_binding_copy(binding);

    found = tsearch((void*) copy, &self->bindings, compare);
    if (!found) {
        LOG_ERROR("Could not store binding!");
        noia_binding_free(copy);
        return;
    }

    LOG_INFO2("Created binding (code: %d, modifiers: %d)",
              binding->code, binding->modifiers);
}

//------------------------------------------------------------------------------

NoiaBinding* noia_mode_find_binding(NoiaMode* self,
                                    int code,
                                    uint32_t modifiers)
{
    NoiaBinding** found = NULL;
    NoiaBinding searched;
    searched.code = code;
    searched.modifiers = modifiers;

    found = tfind((void*) &searched, &self->bindings, compare);
    if (found) {
        return *found;
    }
    return NULL;
}

//------------------------------------------------------------------------------

