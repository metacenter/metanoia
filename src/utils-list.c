// file: utils-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-list.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaList* noia_list_new(NoiaFreeFunc free_data)
{
    NoiaList* self = malloc(sizeof(NoiaList));
    if (!self) {
        return NULL;
    }

    chain_initialize(&self->base, (NoiaFreeFunc) link_free);
    self->free_data = free_data;
    return self;
}

//------------------------------------------------------------------------------

void noia_list_free(NoiaList* self)
{
    if (!self) {
        return;
    }

    noia_list_clean(self);
    free(self);
}

//------------------------------------------------------------------------------

void noia_list_prepend(NoiaList* self, void* data)
{
    if (!self) {
        return;
    }

    chain_prejoin(&self->base, link_new(data));
}

//------------------------------------------------------------------------------

void noia_list_append(NoiaList* self, void* data)
{
    if (!self) {
        return;
    }

    chain_adjoin(&self->base, link_new(data));
}

//------------------------------------------------------------------------------

void* noia_list_pop(NoiaList* self)
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

NoiaResult noia_list_remove(NoiaList* self, void* data, NoiaCompareFunc compare)
{
    if (!self) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
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
        return NOIA_RESULT_NOT_FOUND;
    }

    NoiaResult result = chain_disjoin(&self->base, link);
    if (result == NOIA_RESULT_SUCCESS) {
        link_destroy(link, self->free_data);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_list_remove_all(NoiaList* self,
                                void* data,
                                NoiaCompareFunc compare)
{
    bool removed_all = false;
    while (!removed_all) {
        NoiaResult result = noia_list_remove(self, data, compare);
        if (result == NOIA_RESULT_NOT_FOUND) {
            removed_all = true;
        } else if (result != NOIA_RESULT_SUCCESS) {
            return result;
        }
    }
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_list_clean(NoiaList* self)
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

NoiaList* noia_list_subtract(NoiaList* minuend,
                             NoiaList* subtrahent,
                             NoiaCompareFunc compare)
{
    NoiaList* difference = noia_list_new(minuend->free_data);

    FOR_EACH (minuend, mlink) {
        int found = 0;
        FOR_EACH (subtrahent, slink) {
            found = !compare(mlink->data, slink->data);
        }
        if (!found) {
            noia_list_append(difference, mlink->data);
        }
    }

    return difference;
}

//------------------------------------------------------------------------------

