// file: utils-list.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_LIST_H__
#define __NOIA_UTILS_LIST_H__

#include "utils-chain.h"

#define FOR_EACH(LIST,LINK) \
    for (Link* LINK = ((Chain*) LIST)->first; LINK; LINK = LINK->next)

#define FOR_EACH_REVERSE(LIST,LINK) \
    for (Link* LINK = ((Chain*) LIST)->last; LINK; LINK = LINK->prev)

typedef struct {
    Chain base;
    NoiaFreeFunc free_data;
} NoiaList;

NoiaList* noia_list_new(NoiaFreeFunc free_data);
void noia_list_free(NoiaList* self);

void noia_list_prepend(NoiaList* self, void* data);

void noia_list_append(NoiaList* self, void* data);

void* noia_list_pop(NoiaList* self);

NoiaResult noia_list_remove(NoiaList* self,
                            void* data,
                            NoiaCompareFunc compare);

NoiaResult noia_list_remove_all(NoiaList* self,
                                void* data,
                                NoiaCompareFunc compare);

void noia_list_clean(NoiaList* self);

NoiaList* noia_list_subtract(NoiaList* minuend,
                             NoiaList* subtrahent,
                             NoiaCompareFunc compare);

static inline int noia_list_len(NoiaList* self)
{
    return chain_len((Chain*) self);
}

static inline int noia_list_recalculate_length(NoiaList* self)
{
    return chain_recalculate_length((Chain*) self);
}

static inline Link* noia_list_first(NoiaList* self)
{
    return self->base.first;
}

static inline Link* noia_list_last(NoiaList* self)
{
    return self->base.last;
}

#endif // __NOIA_UTILS_LIST_H__

