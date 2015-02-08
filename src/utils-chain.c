// file: utils-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-chain.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

Link* link_new(void* data)
{
    Link* self = malloc(sizeof(Link));
    link_initialize(self, data);
    return self;
}

//------------------------------------------------------------------------------

void link_free(Link* self)
{
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(Link));
    free(self);
}

//------------------------------------------------------------------------------

void link_destroy(Link* self, AuraFreeFunc free_data)
{
    if (!self) {
        return;
    }
    if (free_data) {
        free_data(self->data);
    }
    memset(self, 0, sizeof(Link));
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

Chain* chain_new(AuraFreeFunc free_link)
{
    Chain* self = malloc(sizeof(Chain));
    if (!self) {
        return NULL;
    }

    chain_initialize(self, free_link);
    return self;
}

//------------------------------------------------------------------------------

void chain_initialize(Chain* self, AuraFreeFunc free_link)
{
    if (!self) {
        return;
    }

    self->first = NULL;
    self->last = NULL;
    self->len = 0;
    self->free_link = free_link;
}

//------------------------------------------------------------------------------

void chain_free(Chain* self)
{
    if (!self) {
        return;
    }

    chain_clean(self);
    free(self);
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
    if (!self) {
        return;
    }

    if (self->free_link) {
        Link* link = self->first;
        while (link) {
            Link* next = link->next;
            self->free_link(link);
            link = next;
        }
    }

    self->first = NULL;
    self->last = NULL;
    self->len = 0;
}

//------------------------------------------------------------------------------

