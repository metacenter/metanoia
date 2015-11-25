// file: utils-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-chain.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaLink* noia_link_new(void* data)
{
    NoiaLink* self = malloc(sizeof(NoiaLink));
    noia_link_initialize(self, data);
    return self;
}

//------------------------------------------------------------------------------

void noia_link_free(NoiaLink* self)
{
    if (!self) {
        return;
    }
    memset(self, 0, sizeof(NoiaLink));
    free(self);
}

//------------------------------------------------------------------------------

void noia_link_destroy(NoiaLink* self, NoiaFreeFunc free_data)
{
    if (!self) {
        return;
    }
    if (free_data) {
        free_data(self->data);
    }
    memset(self, 0, sizeof(NoiaLink));
    free(self);
}

//------------------------------------------------------------------------------

void noia_link_initialize(NoiaLink* self, void* data)
{
    if (!self) {
        return;
    }
    self->prev = NULL;
    self->next = NULL;
    self->data = data;
}

//------------------------------------------------------------------------------

NoiaChain* noia_chain_new(NoiaFreeFunc free_link)
{
    NoiaChain* self = malloc(sizeof(NoiaChain));
    if (!self) {
        return NULL;
    }

    noia_chain_initialize(self, free_link);
    return self;
}

//------------------------------------------------------------------------------

void noia_chain_initialize(NoiaChain* self, NoiaFreeFunc free_link)
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

void noia_chain_free(NoiaChain* self)
{
    if (!self) {
        return;
    }

    noia_chain_clean(self);
    free(self);
}

//------------------------------------------------------------------------------

int noia_chain_len(NoiaChain* self)
{
    if (!self) {
        return 0;
    }

    return self->len;
}

//------------------------------------------------------------------------------

int noia_chain_recalculate_length(NoiaChain* self)
{
    int len = 0;
    for (NoiaLink* link = self->first; link; link = link->next) {
        len += 1;
    }
    return len;
}

//------------------------------------------------------------------------------

void noia_chain_add_first(NoiaChain* self, NoiaLink* link)
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

void noia_chain_prejoin(NoiaChain* self, NoiaLink* link)
{
    if (!self) {
        return;
    }

    if (self->len == 0) {
        noia_chain_add_first(self, link);
    } else {
        link->next = self->first;
        link->prev = NULL;
        self->first->prev = link;
        self->first = link;
        self->len += 1;
    }
}

//------------------------------------------------------------------------------

void noia_chain_adjoin(NoiaChain* self, NoiaLink* link)
{
    if (!self) {
        return;
    }

    if (self->len == 0) {
        noia_chain_add_first(self, link);
    } else {
        link->next = NULL;
        link->prev = self->last;
        self->last->next = link;
        self->last = link;
        self->len += 1;
    }
}

//------------------------------------------------------------------------------

NoiaResult noia_chain_unjoin(NoiaChain* self, NoiaLink* unjoinee)
{
    if (!self || !unjoinee) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    int found = false;
    NoiaLink* link = NULL;
    for (link = self->first; link; link = link->next) {
        found = (link == unjoinee);
        if (found) {
            break;
        }
    }

    if (!found) {
        return NOIA_RESULT_NOT_FOUND;
    }

    NoiaResult result = noia_chain_disjoin(self, link);
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_chain_disjoin(NoiaChain* self, NoiaLink* link)
{
    if (!self || !link) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    NoiaLink* prev = link->prev;
    NoiaLink* next = link->next;

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
    self->len = noia_chain_recalculate_length(self);
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_chain_clean(NoiaChain* self)
{
    if (!self) {
        return;
    }

    if (self->free_link) {
        NoiaLink* link = self->first;
        while (link) {
            NoiaLink* next = link->next;
            self->free_link(link);
            link = next;
        }
    }

    self->first = NULL;
    self->last = NULL;
    self->len = 0;
}

//------------------------------------------------------------------------------

