// file: keyboard-mode.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-mode.h"
#include "keyboard-binding.h"

#include "utils-log.h"

#include <malloc.h>
#include <search.h>

//------------------------------------------------------------------------------

static int compare(const void* data1, const void* data2)
{
    const AuraBinding* binding1 = (const AuraBinding*) data1;
    const AuraBinding* binding2 = (const AuraBinding*) data2;

    if (binding1->modifiers < binding2->modifiers) return -1;
    if (binding1->modifiers > binding2->modifiers) return  1;

    if (binding1->code < binding2->code) return -1;
    if (binding1->code > binding2->code) return  1;

    return 0;
}

//------------------------------------------------------------------------------

AuraMode* aura_mode_new(AuraModeEnum modeid)
{
    AuraMode* self = malloc(sizeof(AuraMode));
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

void aura_mode_add_binding(AuraMode* self, const AuraBinding* binding)
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
    AuraBinding* copy = aura_binding_copy(binding);

    found = tsearch((void*) copy, &self->bindings, compare);
    if (!found) {
        LOG_ERROR("Could not store binding!");
        aura_binding_free(copy);
        return;
    }

    LOG_INFO2("Created binding (code: %d, modifiers: %d)",
              binding->code, binding->modifiers);
}

//------------------------------------------------------------------------------

AuraBinding* aura_mode_find_binding(AuraMode* self,
                                    int code,
                                    uint32_t modifiers)
{
    AuraBinding** found = NULL;
    AuraBinding searched;
    searched.code = code;
    searched.modifiers = modifiers;

    found = tfind((void*) &searched, &self->bindings, compare);
    if (found) {
        return *found;
    }
    return NULL;
}

//------------------------------------------------------------------------------

