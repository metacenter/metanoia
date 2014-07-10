// file: chain.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_CHAIN_H__
#define __AURA_CHAIN_H__

typedef void (*FreeFunc) (void*);

typedef struct Link Link;
struct Link {
    Link *prev;
    Link *next;
    void *data;
};

Link *link_new(void *data);
void link_free(Link* link, FreeFunc freefunc);

typedef struct {
    Link *first;
    Link *last;
    int len;
    FreeFunc freefunc;
} Chain;

Chain *chain_new(FreeFunc freefunc);
void chain_free(Chain* chain);

int chain_len(Chain* chain);

void chain_prepend(Chain* chain, void* data);
void chain_append(Chain* chain, void* data);
void *chain_pop(Chain* chain);

#endif // __AURA_CHAIN_H__

