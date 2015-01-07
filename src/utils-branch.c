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

    self->data = NULL;
    self->subbranches = chain_new(0);
    return self;
}

//------------------------------------------------------------------------------

void aura_branch_free(AuraBranch* self)
{
    if (!self) {
        return;
    }

    chain_free(self->subbranches);
}

//------------------------------------------------------------------------------

