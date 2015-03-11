// file: utils-branch.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-branch.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaBranch* noia_branch_new()
{
    NoiaBranch* self = malloc(sizeof(NoiaBranch));
    if (!self) {
        return NULL;
    }

    link_initialize(&self->base, NULL);
    self->trunk = NULL;
    self->twigs = chain_new(NULL);
    return self;
}

//------------------------------------------------------------------------------

void noia_branch_free(NoiaBranch* self, NoiaFreeFunc free_data)
{
    if (!self) {
        return;
    }

    NoiaBranch* next = NULL;
    NoiaBranch* twig = (NoiaBranch*) self->twigs->first;
    while (twig) {
        next = (NoiaBranch*) twig->base.next;
        noia_branch_free(twig, free_data);
        twig = next;
    }
    chain_free(self->twigs);
    if (free_data) {
        free_data(self->base.data);
    }

    memset(self, 0, sizeof(NoiaBranch));
    free(self);
}

//------------------------------------------------------------------------------

void noia_branch_prepend(NoiaBranch* self, NoiaBranch* other)
{
    if (!self || !other) {
        return;
    }

    other->trunk = self;
    chain_prejoin(self->twigs, (Link*) other);
}

//------------------------------------------------------------------------------

void noia_branch_append(NoiaBranch* self, NoiaBranch* other)
{
    if (!self || !other) {
        return;
    }

    other->trunk = self;
    chain_adjoin(self->twigs, (Link*) other);
}

//------------------------------------------------------------------------------

NoiaResult noia_branch_remove(NoiaBranch* self, NoiaBranch* other)
{
    if (!self || !other) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    other->trunk = NULL;
    return chain_unjoin(self->twigs, (Link*) other);
}

//------------------------------------------------------------------------------

void noia_branch_set_data(NoiaBranch* self, void* data)
{
    if (!self) {
        return;
    }

    self->base.data = data;
}

//------------------------------------------------------------------------------

NoiaBranch* noia_branch_find(NoiaBranch* self,
                             void* data,
                             NoiaBranchCompare compare)
{
    if (!self) {
        return NULL;
    }

    if (compare(self, data) == 0) {
        return self;
    }

    FOR_EACH_TWIG(self, twig) {
        NoiaBranch* branch = noia_branch_find(twig, data, compare);
        if (branch) {
            return branch;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------

