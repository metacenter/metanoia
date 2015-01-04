// file: keyboard-mode.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "keyboard-mode.h"
#include "keyboard-argmand.h"

#include "utils-log.h"

#include <malloc.h>
#include <search.h>

//------------------------------------------------------------------------------

static int compare(const AuraArgmand* argmand1, const AuraArgmand* argmand2)
{
    if (argmand1->modifiers < argmand2->modifiers) return -1;
    if (argmand1->modifiers > argmand2->modifiers) return  1;

    if (argmand1->code < argmand2->code) return -1;
    if (argmand1->code > argmand2->code) return  1;

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
    self->argmands = NULL;
    self->active = 0;
    return self;
}

//------------------------------------------------------------------------------

void aura_mode_add_argmand(AuraMode* self, const AuraArgmand* argmand)
{
    void* found;

    // Skip if already exists
    found = tfind((void *) argmand, &self->argmands, (CompareFunc) compare);
    if (found) {
        LOG_WARN2("Argmand already exists! (code: %d, modifiers: %d)",
                  argmand->code, argmand->modifiers);
        return;
    }

    // Add argmand
    AuraArgmand* copy = aura_argmand_copy(argmand);

    found = tsearch((void *) copy, &self->argmands, (CompareFunc) compare);
    if (!found) {
        LOG_ERROR("Could not store argmand!");
        aura_argmand_free(copy);
        return;
    }

    LOG_INFO2("Created argmand (code: %d, modifiers: %d)",
              argmand->code, argmand->modifiers);
}

//------------------------------------------------------------------------------

AuraArgmand* aura_mode_find_argmand(AuraMode* self,
                                    int code,
                                    uint32_t modifiers)
{
    AuraArgmand** found = NULL;
    AuraArgmand searched;
    searched.code = code;
    searched.modifiers = modifiers;

    found = tfind((void*) &searched, &self->argmands, (CompareFunc) compare);
    if (found) {
        return *found;
    }
    return NULL;
}

//------------------------------------------------------------------------------

