// file: utils-list.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_LIST_H__
#define __AURA_UTILS_LIST_H__

#include "utils-chain.h"

#define FOR_EACH(LIST,LINK) \
    for (Link* LINK = ((Chain*) LIST)->first; LINK; LINK = LINK->next)

typedef struct {
    Chain base;
    AuraFreeFunc free_data;
} AuraList;

AuraList* aura_list_new(AuraFreeFunc free_data);
void aura_list_free(AuraList* self);

void aura_list_prepend(AuraList* self, void* data);

void aura_list_append(AuraList* self, void* data);

void* aura_list_pop(AuraList* self);

AuraResult aura_list_remove(AuraList* self,
                            void* data,
                            AuraCompareFunc compare);

AuraResult aura_list_remove_all(AuraList* self,
                                void* data,
                                AuraCompareFunc compare);

void aura_list_clean(AuraList* self);

AuraList* aura_list_subtract(AuraList* minuend,
                             AuraList* subtrahent,
                             AuraCompareFunc compare);

static inline int aura_list_len(AuraList* self)
{
    return chain_len((Chain*) self);
}

static inline int aura_list_recalculate_length(AuraList* self)
{
    return chain_recalculate_length((Chain*) self);
}

#endif // __AURA_UTILS_LIST_H__

