// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-branch.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaBranch* noia_branch_new(void)
{
    NoiaBranch* self = malloc(sizeof(NoiaBranch));
    noia_branch_initialize(self);
    return self;
}

//------------------------------------------------------------------------------

void noia_branch_initialize(NoiaBranch* self)
{
    self->trunk = NULL;
    self->spatial_twigs = noia_chain_new(NULL);
    self->temporal_twigs = noia_chain_new(NULL);
    noia_link_initialize(&self->spatial_link, self);
    noia_link_initialize(&self->temporal_link, self);
}

//------------------------------------------------------------------------------

void noia_branch_finalize(NoiaBranch* self, NoiaFreeFunc free_data)
{
    NOIA_ENSURE(self, return);

    // Free twigs
    noia_chain_free(self->spatial_twigs);
    noia_chain_free(self->temporal_twigs);

    // Free data if not NULL
    if (free_data) {
        free_data(self->data);
    }

    // Zero and free memory
    memset(self, 0, sizeof(NoiaBranch));
}

//------------------------------------------------------------------------------

void noia_branch_free(NoiaBranch* self, NoiaFreeFunc free_data)
{
    NOIA_ENSURE(self, return);

    // Free recursively whole tree
    NoiaBranch* next = NULL;
    NoiaBranch* twig = noia_branch_get_first(self);
    while (twig) {
        next = noia_branch_get_next(twig);
        noia_branch_free(twig, free_data);
        twig = next;
    }

    // Finalize branch
    noia_branch_finalize(self, free_data);
    free(self);
}

//------------------------------------------------------------------------------

void noia_branch_prepend(NoiaBranch* self, NoiaBranch* other)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(other, return);

    other->trunk = self;
    noia_chain_prejoin(self->spatial_twigs, &other->spatial_link);
    noia_chain_adjoin(self->temporal_twigs, &other->temporal_link);
}

//------------------------------------------------------------------------------

// Add new branch to the end both spatially and temporally.
void noia_branch_append(NoiaBranch* self, NoiaBranch* other)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(other, return);

    other->trunk = self;
    noia_chain_adjoin(self->spatial_twigs, &other->spatial_link);
    noia_chain_adjoin(self->temporal_twigs, &other->temporal_link);
}

//------------------------------------------------------------------------------

// Add new branch spatially inside chain and temporally at the end.
void noia_branch_insert_before(NoiaBranch* self, NoiaBranch* other)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(other, return);
    NOIA_ENSURE(self->trunk, return);

    other->trunk = self->trunk;
    noia_chain_prejoin_onto(self->trunk->spatial_twigs,
                            &other->spatial_link,
                            &self->spatial_link);
    noia_chain_adjoin(self->trunk->temporal_twigs, &other->temporal_link);
}

//------------------------------------------------------------------------------

// Add new branch spatially inside chain and temporally at the end.
void noia_branch_insert_after(NoiaBranch* self, NoiaBranch* other)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(other, return);
    NOIA_ENSURE(self->trunk, return);

    other->trunk = self->trunk;
    noia_chain_adjoin_onto(self->trunk->spatial_twigs,
                           &other->spatial_link,
                           &self->spatial_link);
    noia_chain_adjoin(self->trunk->temporal_twigs, &other->temporal_link);
}

//------------------------------------------------------------------------------

// Remove branch from its trunks spatial and temporal twigs.
void noia_branch_remove(NoiaBranch* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->trunk, return);

    noia_chain_unjoin(self->trunk->spatial_twigs, &self->spatial_link);
    noia_chain_unjoin(self->trunk->temporal_twigs, &self->temporal_link);
    self->trunk = NULL;
}

//------------------------------------------------------------------------------

// Pop is done by removing branch from its trunk and prepending it back again.
// First twig is threated as most recent.
void noia_branch_pop(NoiaBranch* self)
{
    NOIA_ENSURE(self, return);

    NoiaBranch* trunk = self->trunk;
    if (trunk) {
        noia_chain_unjoin(self->trunk->temporal_twigs, &self->temporal_link);
        noia_chain_prejoin(self->trunk->temporal_twigs, &self->temporal_link);
    }
}

//------------------------------------------------------------------------------

// Set user data to `self->data`.
void noia_branch_set_data(NoiaBranch* self, void* data)
{
    NOIA_ENSURE(self, return);
    self->data = data;
}

//------------------------------------------------------------------------------

// Recursively search for branch using `self->data` for comparison.
NoiaBranch* noia_branch_find(NoiaBranch* self,
                             void* data,
                             NoiaBranchCompare compare)
{
    NOIA_ENSURE(self, return NULL);

    NoiaBranch* result = NULL;
    if (compare(self, data) == 0) {
        result = self;
    } else {
        NOIA_FOR_EACH_TWIG (self, twig) {
            NoiaBranch* branch = noia_branch_find(twig, data, compare);
            if (branch) {
                result = branch;
                break;
            }
        }
    }

    return result;
}

//------------------------------------------------------------------------------

