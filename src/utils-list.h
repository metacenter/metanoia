// file: utils-list.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_LIST_H__
#define __NOIA_UTILS_LIST_H__

#include "utils-chain.h"

#define FOR_EACH(LIST,LINK) \
    for (Link* LINK = LIST->base.first; LINK; LINK = LINK->next)

#define FOR_EACH_REVERSE(LIST,LINK) \
    for (Link* LINK = LIST->base.last; LINK; LINK = LINK->prev)

/// Simple implementations of double linked list.
/// This list is meant for storing data of the same type and provide
/// destruction mechanism using data-destructor passed in constructor of list.
///
/// @note When frequent allocation and deallocation of memory is needed,
///       NoiaPool may be a better choise.
/// @see Chain, NoiaPool
typedef struct {
    Chain base;
    NoiaFreeFunc free_data;
} NoiaList;

/// Constructor of NoiaList.
/// @param free_data - destructor for data stored in the list.
NoiaList* noia_list_new(NoiaFreeFunc free_data);

/// Destructor of NoiaList.
void noia_list_free(NoiaList* self);

/// Adds a new element to the beginning of the list.
void noia_list_prepend(NoiaList* self, void* data);

/// Adds a new element to the ending of the list.
void noia_list_append(NoiaList* self, void* data);

/// Removes and return last element of the list.
void* noia_list_pop(NoiaList* self);

/// Searches for first occurrence of given data using given compare function and
/// removes it.
NoiaResult noia_list_remove(NoiaList* self,
                            void* data,
                            NoiaCompareFunc compare);

/// Searches for all occurrences of given data using given compare function and
/// removes all of them.
NoiaResult noia_list_remove_all(NoiaList* self,
                                void* data,
                                NoiaCompareFunc compare);

/// Removes all elements and destroys them if data-destructor function was
/// given in list constructor.
/// @see noia_list_new
void noia_list_clean(NoiaList* self);

/// Return a new list composed of elements of `minuent` that are not contained
/// in `subtrahent`.
/// Uses `compare` function to compare elements and `duplicate` to copy them.
/// `duplicate` can be NULL.
NoiaList* noia_list_subtract(NoiaList* minuend,
                             NoiaList* subtrahent,
                             NoiaCompareFunc compare,
                             NoiaDuplicateFunc duplicate);

/// Return the length of the list.
static inline int noia_list_len(NoiaList* self)
{
    return chain_len((Chain*) self);
}

/// Recalculate and return the length of the list.
static inline int noia_list_recalculate_length(NoiaList* self)
{
    return chain_recalculate_length((Chain*) self);
}

/// Return first element of the list.
static inline Link* noia_list_first(NoiaList* self)
{
    return self->base.first;
}

/// Return last element of the list.
static inline Link* noia_list_last(NoiaList* self)
{
    return self->base.last;
}

#endif // __NOIA_UTILS_LIST_H__

