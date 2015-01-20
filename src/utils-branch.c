// file: utils-branch.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-branch.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraBranch* aura_branch_new()
{
    AuraBranch* self = malloc(sizeof(AuraBranch));
    if (!self) {
        LOG_ERROR("Memory allocation failed!");
        return NULL;
    }

    link_initialize(&self->link, NULL);
    self->trunk = NULL;
    self->twigs = chain_new(0);
    return self;
}

//------------------------------------------------------------------------------

void aura_branch_free(AuraBranch* self)
{
    if (!self) {
        return;
    }

    chain_free(self->twigs);
}

//------------------------------------------------------------------------------

void aura_branch_prepend(AuraBranch* self, AuraBranch* other)
{
    if (!self) {
        return;
    }

    chain_prejoin(self->twigs, other);
    other->trunk = self;
}

//------------------------------------------------------------------------------

void aura_branch_append(AuraBranch* self, AuraBranch* other)
{
    if (!self) {
        return;
    }

    chain_adjoin(self->twigs, other);
    other->trunk = self;
}

//------------------------------------------------------------------------------

void aura_branch_set_data(AuraBranch* self, void* data)
{
    if (!self) {
        return;
    }

    self->link.data = data;
}

//------------------------------------------------------------------------------

