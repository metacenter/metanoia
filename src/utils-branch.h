// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_BRANCH_H
#define NOIA_UTILS_BRANCH_H

#include "utils-chain.h"

#include <stddef.h>

//------------------------------------------------------------------------------

/// Structure for representing tree-like structure with temporal order.
typedef struct NoiaBranch NoiaBranch;
struct NoiaBranch {
    NoiaBranch* trunk;
    NoiaChain* spatial_twigs;
    NoiaChain* temporal_twigs;
    NoiaLink spatial_link;
    NoiaLink temporal_link;
    void* data;
};

/// Branch Compare function.
/// @see noia_branch_find
typedef int (*NoiaBranchCompare) (NoiaBranch*, void*);

//------------------------------------------------------------------------------

/// Iterate over all twigs of a branch in time order
/// (from most recently used to most formerly used).
#define NOIA_FOR_EACH_TWIG(TRUNK,BRANCH) \
    for (NoiaBranch* BRANCH = noia_branch_get_recent(TRUNK); \
    BRANCH; BRANCH = noia_branch_get_later(BRANCH))

/// Iterate over all twigs of a branch in reverse time order
/// (from most formerly used to most recently used).
#define NOIA_FOR_EACH_TWIG_REVERSED(TRUNK,BRANCH) \
    for (NoiaBranch* BRANCH = noia_branch_get_former(TRUNK); \
    BRANCH; BRANCH = noia_branch_get_earlier(BRANCH))

//------------------------------------------------------------------------------

/// Branch constructor.
NoiaBranch* noia_branch_new(void);

/// Branch initialization.
void noia_branch_initialize(NoiaBranch* self);

/// Deallocate contents of branch.
void noia_branch_finalize(NoiaBranch* self, NoiaFreeFunc free_data);

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
void noia_branch_remove(NoiaBranch* self);

/// Make the branch first (most recently used) in terms of temporal order.
void noia_branch_pop(NoiaBranch* self);

/// Set user data for branch `self`.
void noia_branch_set_data(NoiaBranch* self, void* data);

/// Find branch with given data using compare function.
NoiaBranch* noia_branch_find(NoiaBranch* self,
                             void* data,
                             NoiaBranchCompare compare);

/// Get number of twigs.
static inline int noia_branch_get_number_of_twigs(NoiaBranch* self)
{
    return noia_chain_len(self->spatial_twigs);
}

//------------------------------------------------------------------------------

/// Get first child in spatial terms.
static inline NoiaBranch* noia_branch_get_first(NoiaBranch* self)
{
    NoiaLink* link = self->spatial_twigs->first;
    return (NoiaBranch*) (link ? link->data : NULL);
}

/// Get last child in spatial terms.
static inline NoiaBranch* noia_branch_get_last(NoiaBranch* self)
{
    NoiaLink* link = self->spatial_twigs->last;
    return (NoiaBranch*) (link ? link->data : NULL);
}

/// Get first child in temporal terms (most recently used).
static inline NoiaBranch* noia_branch_get_recent(NoiaBranch* self)
{
    NoiaLink* link = self->temporal_twigs->first;
    return (NoiaBranch*) (link ? link->data : NULL);
}

/// Get last child in temporal terms (most formerly used).
static inline NoiaBranch* noia_branch_get_former(NoiaBranch* self)
{
    NoiaLink* link = self->temporal_twigs->last;
    return (NoiaBranch*) (link ? link->data : NULL);
}

//------------------------------------------------------------------------------

/// Get next branch from twigs of `self`'s trunk.
static inline NoiaBranch* noia_branch_get_next(NoiaBranch* self)
{
    NoiaLink* link = self->spatial_link.next;
    return (NoiaBranch*) (link ? link->data : NULL);
}

/// Get previous branch from twigs of `self`'s trunk.
static inline NoiaBranch* noia_branch_get_previous(NoiaBranch* self)
{
    NoiaLink* link = self->spatial_link.prev;
    return (NoiaBranch*) (link ? link->data : NULL);
}

/// Get later branch (next in terms in temporal order).
static inline NoiaBranch* noia_branch_get_later(NoiaBranch* self)
{
    NoiaLink* link = self->temporal_link.next;
    return (NoiaBranch*) (link ? link->data : NULL);
}

/// Get earlier branch (previous in terms in temporal order).
static inline NoiaBranch* noia_branch_get_earlier(NoiaBranch* self)
{
    NoiaLink* link = self->temporal_link.prev;
    return (NoiaBranch*) (link ? link->data : NULL);
}

//------------------------------------------------------------------------------

#endif // NOIA_UTILS_BRANCH_H

