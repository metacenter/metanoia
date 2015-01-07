// file: utils-branch.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_BRANCH_H__
#define __AURA_UTILS_BRANCH_H__

#include "utils-chain.h"

typedef struct {
    void* data;
    Chain* subbranches;
} AuraBranch;

AuraBranch* aura_branch_new();
void aura_branch_free();

#endif // __AURA_UTILS_BRANCH_H__

