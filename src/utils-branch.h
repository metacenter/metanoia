// file: utils-branch.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_BRANCH_H__
#define __NOIA_UTILS_BRANCH_H__

#include "utils-chain.h"

#define FOR_EACH_TWIG(TRUNK,BRANCH) \
    for (NoiaBranch* BRANCH = (NoiaBranch*) TRUNK->twigs->first; \
    BRANCH; BRANCH = (NoiaBranch*) BRANCH->base.next)

typedef int (*NoiaBranchCompare) (void*, void*);

typedef struct NoiaBranch NoiaBranch;

/// @todo Unit tests for NoiaBranch
struct NoiaBranch {
    Link base;
    NoiaBranch* trunk;
    Chain* twigs;
};

NoiaBranch* noia_branch_new();
void noia_branch_free(NoiaBranch* self, NoiaFreeFunc free_data);

void noia_branch_prepend(NoiaBranch* self, NoiaBranch* other);
void noia_branch_append(NoiaBranch* self, NoiaBranch* other);

NoiaResult noia_branch_remove(NoiaBranch* self, NoiaBranch* other);

void noia_branch_set_data(NoiaBranch* self, void* data);

NoiaBranch* noia_branch_find(NoiaBranch* self,
                             void* data,
                             NoiaBranchCompare compare);

#endif // __NOIA_UTILS_BRANCH_H__

