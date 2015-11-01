// file: utils-pool.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_POOL_H__
#define __NOIA_UTILS_POOL_H__

#include "stddef.h"

#define NOIA_ITERATE_POOL(POOL,INDEX,ELEMENT) \
    for (INDEX = 0, ELEMENT = noia_pool_get(POOL, INDEX); \
         INDEX < noia_pool_get_size(POOL); \
         ++INDEX, ELEMENT = noia_pool_get(POOL, INDEX))

#define NOIA_ITERATE_POOL_REVERSE(POOL,INDEX,ELEMENT) \
    for (INDEX = noia_pool_get_size(POOL) - 1, \
         ELEMENT = noia_pool_get(POOL, INDEX); \
         INDEX >= 0; --INDEX, ELEMENT = noia_pool_get(POOL, INDEX))

/// This data type is used to define an array-like structure which can grow
/// when needed.
/// Under the hood it is implemented as an growing array of static arrays.
/// Pointers to elements stay valid when pool grows.
/// This data type should be used to save CPU when frequent memory allocation
/// and deallocation is needed.
typedef struct NoiaPoolStruct NoiaPool;

/// Pool constructor.
/// @param array_size - defines initial pool size and size of memory allocated
///        every time the pool grows.
/// @param type_size - size of data type stored in pool.
NoiaPool* noia_pool_create(unsigned array_size, size_t type_size);

/// Pool destructor.
void noia_pool_destroy(NoiaPool* self);

/// Set default element returned when requested not existing element.
void noia_pool_set_defaut(NoiaPool* self, void* element);

/// Return pointer to next not used element and grow pool if necessary.
void* noia_pool_add(NoiaPool* self);

/// Return element defined by `index`.
void* noia_pool_get(NoiaPool* self, unsigned index);

/// Frees all memory and resets pools `array size` to current allocation size.
/// After call to this function all pointers returned previously should be
/// considered invalid.
void noia_pool_release(NoiaPool* self);

/// Get number of currently stored elements.
unsigned noia_pool_get_size(NoiaPool* self);

/// Get number of element which could be stored without growing the pool.
unsigned noia_pool_get_alocation_size(NoiaPool* self);

#endif // __NOIA_UTILS_POOL_H__

