// file: utils-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-chain.h"

#include <malloc.h>

//------------------------------------------------------------------------------

Link* link_new(void* data)
{
    Link* self = malloc(sizeof(Link));
    if (!self) {
        return NULL;
    }
    self->prev = NULL;
    self->next = NULL;
    self->data = data;
    return self;
}

//------------------------------------------------------------------------------

void link_free(Link* self, FreeFunc freefunc)
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

Chain* chain_new(FreeFunc freefunc)
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

    Link *next, *iter = self->first;
    while (iter) {
        next = iter->next;
        link_free(iter, self->freefunc);
        iter = next;
    }
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

void chain_add_first(Chain* self, void* data)
{
    if (!self) {
        return;
    }

    Link* link = link_new(data);
    self->first = link;
    self->last = link;
    self->len = 1;
}

//------------------------------------------------------------------------------

void chain_prepend(Chain* self, void* data)
{
    if (!self) {
        return;
    }

    if (self->len == 0) {
        chain_add_first(self, data);
    } else {
        Link* link = link_new(data);
        link->next = self->first;
        self->first->prev = link;
        self->first = link;
        self->len += 1;
    }
}

//------------------------------------------------------------------------------

void chain_append(Chain* self, void* data)
{
    if (!self) {
        return;
    }

    if (self->len == 0) {
        chain_add_first(self, data);
    } else {
        Link* link = link_new(data);
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

