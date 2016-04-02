// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_BRANCH_H
#define NOIA_UTILS_BRANCH_H

#include "utils-chain.h"

/// Structure for representing tree-like structure.
typedef struct NoiaBranch NoiaBranch;
struct NoiaBranch {
    NoiaLink base;
    NoiaBranch* trunk;
    NoiaChain* twigs;
};

/// Iterate over all twigs of a branch
#define FOR_EACH_TWIG(TRUNK,BRANCH) \
    for (NoiaBranch* BRANCH = (NoiaBranch*) TRUNK->twigs->first; \
    BRANCH; BRANCH = (NoiaBranch*) BRANCH->base.next)

/// Branch Compare function.
/// @see noia_branch_find
typedef int (*NoiaBranchCompare) (void*, void*);

/// Branch constructor.
NoiaBranch* noia_branch_new(void);

/// Branch destructor.
/// Destructs whole tree and free all data.
void noia_branch_free(NoiaBranch* self, NoiaFreeFunc free_data);

/// Prepend branch `other` to twigs of `self`.
void noia_branch_prepend(NoiaBranch* self, NoiaBranch* other);

/// Append branch `other` to twigs of `self`.
void noia_branch_append(NoiaBranch* self, NoiaBranch* other);

/// Insert `other` to twigs of `self`'s trunk just before `self`.
void noia_branch_insert_before(NoiaBranch* self, NoiaBranch* other);

/// Insert `other` to twigs of `self`'s trunk just after `self`.
void noia_branch_insert_after(NoiaBranch* self, NoiaBranch* other);

/// Remove branch `other` from twigs of `self`.
NoiaResult noia_branch_remove(NoiaBranch* self, NoiaBranch* other);

/// Set user data for branch `self`.
void noia_branch_set_data(NoiaBranch* self, void* data);

/// Find branch with given data using compare function.
NoiaBranch* noia_branch_find(NoiaBranch* self,
                             void* data,
                             NoiaBranchCompare compare);

/// Get next branch from twigs of `self`'s trunk.
static inline NoiaBranch* noia_branch_get_next(NoiaBranch* self)
{
    return (NoiaBranch*) self->base.next;
}

/// Get previous branch from twigs of `self`'s trunk.
static inline NoiaBranch* noia_branch_get_previous(NoiaBranch* self)
{
    return (NoiaBranch*) self->base.prev;
}

#endif // NOIA_UTILS_BRANCH_H

