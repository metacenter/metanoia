// file: utils-branch.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_BRANCH_H__
#define __AURA_UTILS_BRANCH_H__

#include "utils-chain.h"

#define FOR_EACH_TWIG(TRUNK,BRANCH) \
    for (AuraBranch* BRANCH = (AuraBranch*) TRUNK->twigs->first; \
    BRANCH; BRANCH = (AuraBranch*) BRANCH->base.next)

typedef int (*AuraBranchCompare) (void*, void*);

typedef struct AuraBranch AuraBranch;

/// @todo Unit tests for AuraBranch
struct AuraBranch {
    Link base;
    AuraBranch* trunk;
    Chain* twigs;
};

AuraBranch* aura_branch_new();
void aura_branch_free(AuraBranch* self, AuraFreeFunc free_data);

void aura_branch_prepend(AuraBranch* self, AuraBranch* other);
void aura_branch_append(AuraBranch* self, AuraBranch* other);

AuraResult aura_branch_remove(AuraBranch* self, AuraBranch* other);

void aura_branch_set_data(AuraBranch* self, void* data);

AuraBranch* aura_branch_find(AuraBranch* self,
                             void* data,
                             AuraBranchCompare compare);

#endif // __AURA_UTILS_BRANCH_H__

