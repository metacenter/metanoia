// file: utils-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-list.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

AuraList* aura_list_new(AuraFreeFunc free_data)
{
    AuraList* self = malloc(sizeof(AuraList));
    if (!self) {
        return NULL;
    }

    chain_initialize(&self->base, (AuraFreeFunc) link_free);
    self->free_data = free_data;
    return self;
}

//------------------------------------------------------------------------------

void aura_list_free(AuraList* self)
{
    if (!self) {
        return;
    }

    aura_list_clean(self);
    free(self);
}

//------------------------------------------------------------------------------

void aura_list_prepend(AuraList* self, void* data)
{
    if (!self) {
        return;
    }

    chain_prejoin(&self->base, link_new(data));
}

//------------------------------------------------------------------------------

void aura_list_append(AuraList* self, void* data)
{
    if (!self) {
        return;
    }

    chain_adjoin(&self->base, link_new(data));
}

//------------------------------------------------------------------------------

void* aura_list_pop(AuraList* self)
{
    void* result;
    Link* next;

    if (!self || self->base.len == 0) {
        return NULL;
    }

    result = self->base.first->data;
    next = self->base.first->next;
    link_free(self->base.first);
    self->base.first = next;
    self->base.len -= 1;

    if (self->base.len == 0) {
        self->base.last = NULL;
    }
    return result;
}

//------------------------------------------------------------------------------

AuraResult aura_list_remove(AuraList* self, void* data, AuraCompareFunc compare)
{
    if (!self) {
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    int found = false;
    Link* link = NULL;
    for (link = self->base.first; link; link = link->next) {
        found = !compare(data, link->data);
        if (found) {
            break;
        }
    }

    if (!found) {
        return AURA_RESULT_NOT_FOUND;
    }

    AuraResult result = chain_disjoin(&self->base, link);
    if (result == AURA_RESULT_SUCCESS) {
        link_destroy(link, self->free_data);
    }
    return result;
}

//------------------------------------------------------------------------------

AuraResult aura_list_remove_all(AuraList* self,
                                void* data,
                                AuraCompareFunc compare)
{
    bool removed_all = false;
    while (!removed_all) {
        AuraResult result = aura_list_remove(self, data, compare);
        if (result == AURA_RESULT_NOT_FOUND) {
            removed_all = true;
        } else if (result != AURA_RESULT_SUCCESS) {
            return result;
        }
    }
    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void aura_list_clean(AuraList* self)
{
    Link* iter = self->base.first;
    while (iter) {
        Link* next = iter->next;
        link_destroy(iter, self->free_data);
        iter = next;
    }

    self->base.first = NULL;
    self->base.last = NULL;
    self->base.len = 0;
}

//------------------------------------------------------------------------------

AuraList* aura_list_subtract(AuraList* minuend,
                             AuraList* subtrahent,
                             AuraCompareFunc compare)
{
    AuraList* difference = aura_list_new(minuend->free_data);

    FOR_EACH (minuend, mlink) {
        int found = 0;
        FOR_EACH (subtrahent, slink) {
            found = !compare(mlink->data, slink->data);
        }
        if (!found) {
            aura_list_append(difference, mlink->data);
        }
    }

    return difference;
}

//------------------------------------------------------------------------------

