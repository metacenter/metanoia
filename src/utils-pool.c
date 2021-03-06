// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-pool.h"
#include "global-types.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------
// PRIVATE

/// Structure defining private members of NoiaPool.
/// @see NoiaPool
struct NoiaPoolStruct {
    /// Array of arrays of stored elements.
    void** pool;

    /// Number of arrays in pool.
    unsigned pool_size;

    /// Number of currently available elements.
    unsigned num_elements;

    /// Numer of elements in one array.
    unsigned array_size;

    /// Size of type of stored data.
    size_t type_size;
};

//------------------------------------------------------------------------------

/// Grows pool by allocating new array of size defined in NoiaPool.
/// @see noia_pool_add, noia_pool_create, noia_pool_release
void noia_pool_grow(NoiaPool* self)
{
    self->pool = realloc(self->pool, sizeof(void*) * (self->pool_size + 1));
    self->pool[self->pool_size] = calloc(self->array_size, self->type_size);
    self->pool_size += 1;
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaPool* noia_pool_create(unsigned array_size, size_t type_size)
{
    NoiaPool* self = calloc(1, sizeof(NoiaPool));
    NOIA_ENSURE(self, abort());

    self->pool = NULL;
    self->pool_size = 0;
    self->num_elements = 0;
    self->array_size = array_size;
    self->type_size = type_size;

    noia_pool_grow(self);
    return self;
}

//------------------------------------------------------------------------------

void noia_pool_destroy(NoiaPool* self)
{
    NOIA_ENSURE(self, return);

    for (unsigned i = 0; i < self->pool_size; ++i) {
        free(self->pool[i]);
    }

    free(self->pool);
    memset(self, 0, sizeof(NoiaPool));
    free(self);
}

//------------------------------------------------------------------------------

void* noia_pool_add(NoiaPool* self)
{
    if (noia_pool_get_alocation_size(self) < self->num_elements + 1) {
        noia_pool_grow(self);
    }

    self->num_elements += 1;
    return noia_pool_get(self, self->num_elements-1);
}

//------------------------------------------------------------------------------

void* noia_pool_get(NoiaPool* self, unsigned index)
{
    void* result = NULL;
    NOIA_BLOCK {
        if (self->num_elements <= index) {
            break;
        }

        unsigned array_num = index / self->array_size;
        unsigned element_num = index % self->array_size;
        result = (void*) ((intptr_t) self->pool[array_num]
                         + self->type_size * element_num);
    }
    return result;
}

//------------------------------------------------------------------------------

void* noia_pool_top(NoiaPool* self)
{
    return noia_pool_get(self, self->num_elements-1);
}

//------------------------------------------------------------------------------

void noia_pool_drop(NoiaPool* self, unsigned num_droped_elements)
{
    self->num_elements -= num_droped_elements;
}

//------------------------------------------------------------------------------

void noia_pool_release(NoiaPool* self)
{
    if (self->pool_size > 1) {
        for (unsigned i = 0; i < self->pool_size; ++i) {
            free(self->pool[i]);
        }
        free(self->pool);

        self->array_size = noia_pool_get_alocation_size(self);
        self->pool = NULL;
        self->pool_size = 0;

        noia_pool_grow(self);
    }

    self->num_elements = 0;
}

//------------------------------------------------------------------------------

unsigned noia_pool_get_size(NoiaPool* self)
{
    return self->num_elements;
}

//------------------------------------------------------------------------------

unsigned noia_pool_get_alocation_size(NoiaPool* self)
{
    return self->array_size * self->pool_size;
}

//------------------------------------------------------------------------------

