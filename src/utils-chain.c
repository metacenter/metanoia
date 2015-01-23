// file: utils-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-chain.h"

#include <malloc.h>

//------------------------------------------------------------------------------

Link* link_new(void* data)
{
    Link* self = malloc(sizeof(Link));
    link_initialize(self, data);
    return self;
}

//------------------------------------------------------------------------------

void link_free(Link* self, AuraFreeFunc freefunc)
{
    if (!self) {
        return;
    }
    if (freefunc) {
        freefunc(self->data);
    }
    free(self);
}

//------------------------------------------------------------------------------

void link_initialize(Link* self, void* data)
{
    if (!self) {
        return;
    }
    self->prev = NULL;
    self->next = NULL;
    self->data = data;
}

//------------------------------------------------------------------------------

Chain* chain_new(AuraFreeFunc freefunc)
{
    Chain* self = malloc(sizeof(Chain));
    if (!self) {
        return NULL;
    }

    self->first = NULL;
    self->last = NULL;
    self->len = 0;
    self->freefunc = freefunc;
    return self;
}

//------------------------------------------------------------------------------

void chain_free(Chain* self)
{
    if (!self) {
        return;
    }

    chain_clean(self);
}

//------------------------------------------------------------------------------

int chain_len(Chain* self)
{
    if (!self) {
        return 0;
    }

    return self->len;
}

//------------------------------------------------------------------------------

int chain_recalculate_length(Chain* self)
{
    int len = 0;
    for (Link* link = self->first; link; link = link->next) {
        len += 1;
    }
    return len;
}

//------------------------------------------------------------------------------

void chain_add_first(Chain* self, Link* link)
{
    if (!self) {
        return;
    }

    self->first = link;
    self->last = link;
    link->prev = NULL;
    link->next = NULL;
    self->len = 1;
}

//------------------------------------------------------------------------------

void chain_prepend(Chain* self, void* data)
{
    if (!self) {
        return;
    }

    chain_prejoin(self, link_new(data));
}

//------------------------------------------------------------------------------

void chain_append(Chain* self, void* data)
{
    if (!self) {
        return;
    }

    chain_adjoin(self, link_new(data));
}

//------------------------------------------------------------------------------

void chain_prejoin(Chain* self, Link* link)
{
    if (!self) {
        return;
    }

    if (self->len == 0) {
        chain_add_first(self, link);
    } else {
        link->next = self->first;
        link->prev = NULL;
        self->first->prev = link;
        self->first = link;
        self->len += 1;
    }
}

//------------------------------------------------------------------------------

void chain_adjoin(Chain* self, Link* link)
{
    if (!self) {
        return;
    }

    if (self->len == 0) {
        chain_add_first(self, link);
    } else {
        link->next = NULL;
        link->prev = self->last;
        self->last->next = link;
        self->last = link;
        self->len += 1;
    }
}

//------------------------------------------------------------------------------

void* chain_pop(Chain* self)
{
    void* result;
    Link* next;

    if (!self || self->len == 0) {
        return NULL;
    }

    result = self->first->data;
    next = self->first->next;
    link_free(self->first, self->freefunc);
    self->first = next;
    self->len -= 1;

    if (self->len == 0) {
        self->last = NULL;
    }
    return result;
}

//------------------------------------------------------------------------------

AuraResult chain_remove(Chain* self, void* data, AuraCompareFunc compare)
{
    if (!self) {
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    int found = 0;
    Link* link = NULL;
    for (link = self->first; link; link = link->next) {
        found = !compare(data, link->data);
        if (found) {
            break;
        }
    }

    if (!found) {
        return AURA_RESULT_NOT_FOUND;
    }

    AuraResult result = chain_disjoin(self, link);
    if (result == AURA_RESULT_SUCCESS) {
        link_free(link, self->freefunc);
    }
    return result;
}

//------------------------------------------------------------------------------

AuraResult chain_unjoin(Chain* self, Link* unjoinee)
{
    if (!self || !unjoinee) {
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    int found = false;
    Link* link = NULL;
    for (link = self->first; link; link = link->next) {
        found = (link == unjoinee);
        if (found) {
            break;
        }
    }

    if (!found) {
        return AURA_RESULT_NOT_FOUND;
    }

    AuraResult result = chain_disjoin(self, link);
    if (result == AURA_RESULT_SUCCESS) {
        link_free(link, self->freefunc);
    }
    return result;
}

//------------------------------------------------------------------------------

AuraResult chain_disjoin(Chain* self, Link* link)
{
    if (!self || !link) {
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    Link* prev = link->prev;
    Link* next = link->next;

    if (prev) {
        prev->next = next;
    } else {
        self->first = next;
    }

    if (next) {
        next->prev = prev;
    } else {
        self->last = prev;
    }

    link->prev = NULL;
    link->next = NULL;
    self->len = chain_recalculate_length(self);
    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void chain_clean(Chain* self)
{
    Link* iter = self->first;
    while (iter) {
        Link* next = iter->next;
        link_free(iter, self->freefunc);
        iter = next;
    }
}

//------------------------------------------------------------------------------

Chain* chain_subtract(Chain* minuend,
                      Chain* subtrahent,
                      AuraCompareFunc compare)
{
    Chain* difference = chain_new(minuend->freefunc);

    Link* mlink;
    for (mlink = minuend->first; mlink; mlink = mlink->next) {
        int found = 0;
        Link* slink;
        for (slink = subtrahent->first; slink && !found; slink = slink->next) {
            found = !compare(mlink->data, slink->data);
        }
        if (!found) {
            chain_append(difference, mlink->data);
        }
    }

    return difference;
}

//------------------------------------------------------------------------------

