// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "input-mode.h"
#include "input-binding.h"

#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

#define __USE_GNU
#define _GNU_SOURCE
#include <search.h>

//------------------------------------------------------------------------------

NoiaMode* noia_mode_create(NoiaModeEnum modeid)
{
    NoiaMode* self = malloc(sizeof(NoiaMode));
    NOIA_ENSURE(self, abort());

    self->modeid = modeid;
    self->bindings = NULL;
    self->active = 0;
    return self;
}

//------------------------------------------------------------------------------

void noia_mode_destroy(NoiaMode* self)
{
    NOIA_ENSURE(self, return);

    tdestroy(self->bindings, (NoiaFreeFunc) noia_binding_free);
    memset(self, 0, sizeof(NoiaMode));
    free(self);
}

//------------------------------------------------------------------------------

void noia_mode_add_binding(NoiaMode* self, const NoiaBinding* binding)
{
    // Skip if already exists
    void* found = tfind((void*) binding, &self->bindings,
                        (NoiaCompareFunc) noia_binding_compare);
    if (found) {
        LOG_WARN2("Binding already exists! (code: %d, modifiers: %d)",
                  binding->code, binding->modifiers);
        return;
    }

    // Add binding
    NoiaBinding* copy = noia_binding_copy(binding);

    found = tsearch((void*) copy, &self->bindings,
                    (NoiaCompareFunc) noia_binding_compare);
    if (not found) {
        LOG_ERROR("Could not store binding!");
        noia_binding_free(copy);
        return;
    }

    LOG_INFO3("Created binding (code: %d, modifiers: %d)",
              binding->code, binding->modifiers);
}

//------------------------------------------------------------------------------

NoiaBinding* noia_mode_find_binding(NoiaMode* self,
                                    int code,
                                    uint32_t modifiers)
{
    NoiaBinding* result = NULL;
    NoiaBinding searched;
    searched.code = code;
    searched.modifiers = modifiers;

    NoiaBinding** found = tfind((void*) &searched, &self->bindings,
                                (NoiaCompareFunc) noia_binding_compare);
    if (found) {
        result = *found;
    }
    return result;
}

//------------------------------------------------------------------------------

