// file: utils-chain.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_CHAIN_H__
#define __AURA_UTILS_CHAIN_H__

// TODO: use 'aura' prefix
// TODO: ref and unref instead of free
typedef void (*AuraFreeFunc) (void*);
typedef int (*AuraCompareFunc) (void*, void*);

typedef struct Link Link;
struct Link {
    Link* prev;
    Link* next;
    void* data;
};

Link* link_new(void* data);
void link_free(Link* link, AuraFreeFunc freefunc);

typedef struct {
    Link* first;
    Link* last;
    int len;
    AuraFreeFunc freefunc;
} Chain;

Chain* chain_new(AuraFreeFunc freefunc);
void chain_free(Chain* chain);

int chain_len(Chain* chain);

void chain_prepend(Chain* chain, void* data);
void chain_append(Chain* chain, void* data);
void* chain_pop(Chain* chain);
void chain_remove(Chain* self, void* data, AuraCompareFunc compare);

Chain* chain_subtract(Chain* minuend,
                      Chain* subtrahent,
                      AuraCompareFunc compare);

#endif // __AURA_UTILS_CHAIN_H__

