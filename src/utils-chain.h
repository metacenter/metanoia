// file: utils-chain.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_CHAIN_H__
#define __NOIA_UTILS_CHAIN_H__

#include "global-constants.h"

typedef struct Link Link;
struct Link {
    Link* prev;
    Link* next;
    void* data;
};

Link* link_new(void* data);
void link_free(Link* link);
void link_destroy(Link* link, NoiaFreeFunc free_data);

void link_initialize(Link* link, void* data);

typedef struct {
    Link* first;
    Link* last;
    int len;
    NoiaFreeFunc free_link;
} Chain;

Chain* chain_new(NoiaFreeFunc free_link);
void chain_free(Chain* self);

void chain_initialize(Chain* self, NoiaFreeFunc free_link);

int chain_len(Chain* self);
int chain_recalculate_length(Chain* self);

void chain_prejoin(Chain* self, Link* link);
void chain_adjoin(Chain* self, Link* link);

NoiaResult chain_unjoin(Chain* self, Link* link);
NoiaResult chain_disjoin(Chain* self, Link* link);

void chain_clean(Chain* self);

#endif // __NOIA_UTILS_CHAIN_H__

