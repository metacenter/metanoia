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
    if (!self || !other) {
        return;
    }

    other->trunk = self;
    chain_prejoin(self->twigs, (Link*) other);
}

//------------------------------------------------------------------------------

void aura_branch_append(AuraBranch* self, AuraBranch* other)
{
    if (!self || !other) {
        return;
    }

    other->trunk = self;
    chain_adjoin(self->twigs, (Link*) other);
}

//------------------------------------------------------------------------------

AuraResult aura_branch_remove(AuraBranch* self, AuraBranch* other)
{
    if (!self || !other) {
        return AURA_RESULT_INCORECT_ARGUMENT;
    }

    other->trunk = NULL;
    return chain_unjoin(self->twigs, (Link*) other);
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

AuraBranch* aura_branch_find(AuraBranch* self,
                             void* data,
                             AuraBranchCompare compare)
{
    if (!self) {
        return NULL;
    }

    if (compare(self, data) == 0) {
        return self;
    }

    FOR_EACH_TWIG(self, twig) {
        AuraBranch* branch = aura_branch_find(twig, data, compare);
        if (branch) {
            return branch;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------

