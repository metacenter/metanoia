// file: utils-chain.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_CHAIN_H__
#define __NOIA_UTILS_CHAIN_H__

#include "global-constants.h"

typedef struct NoiaLink NoiaLink;
struct NoiaLink {
    NoiaLink* prev;
    NoiaLink* next;
    void* data;
};

NoiaLink* noia_link_new(void* data);
void noia_link_free(NoiaLink* link);
void noia_link_destroy(NoiaLink* link, NoiaFreeFunc free_data);

void noia_link_initialize(NoiaLink* link, void* data);

typedef struct {
    NoiaLink* first;
    NoiaLink* last;
    int len;
    NoiaFreeFunc free_link;
} NoiaChain;

NoiaChain* noia_chain_new(NoiaFreeFunc free_link);
void noia_chain_free(NoiaChain* self);

void noia_chain_initialize(NoiaChain* self, NoiaFreeFunc free_link);

int noia_chain_len(NoiaChain* self);
int noia_chain_recalculate_length(NoiaChain* self);

void noia_chain_prejoin(NoiaChain* self, NoiaLink* link);
void noia_chain_adjoin(NoiaChain* self, NoiaLink* link);

NoiaResult noia_chain_unjoin(NoiaChain* self, NoiaLink* link);
NoiaResult noia_chain_disjoin(NoiaChain* self, NoiaLink* link);

void noia_chain_clean(NoiaChain* self);

#endif // __NOIA_UTILS_CHAIN_H__

