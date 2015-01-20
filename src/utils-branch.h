// file: utils-branch.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_BRANCH_H__
#define __AURA_UTILS_BRANCH_H__

#include "utils-chain.h"

typedef struct AuraBranch AuraBranch;

struct AuraBranch {
    Link link;
    AuraBranch* trunk;
    Chain* twigs;
};

AuraBranch* aura_branch_new();
void aura_branch_free();

void aura_branch_prepend(AuraBranch* self, AuraBranch* other);
void aura_branch_append(AuraBranch* self, AuraBranch* other);

void aura_branch_set_data(AuraBranch* self, void* data);

#endif // __AURA_UTILS_BRANCH_H__

