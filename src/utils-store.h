// file: utils-store.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_STORE_H__
#define __AURA_STORE_H__

#include <stdint.h>

typedef uint32_t AuraItemId;

typedef struct {
    union {
        AuraItemId id;
        char* str;
        void* data;
    };
} AuraItem;

typedef int (*AuraStoreCompareFunc) (const void*, const void*);

typedef struct AuraStorePriv AuraStore;


static const AuraItemId scInvalidItemId = 0x0000;
static const AuraItemId scValidItemMask = 0xFFFC;

AuraStore* aura_store_new(AuraStoreCompareFunc compare);
AuraStore* aura_store_new_for_id();
AuraStore* aura_store_new_for_str();
AuraStore* aura_store_new_for_data();

void aura_store_free(AuraStore* self);

AuraItemId aura_store_generate_new_id(AuraStore* self);

int aura_store_add_with_id(AuraStore* self, AuraItemId key, void* data);
int aura_store_add_with_str(AuraStore* self, char* key, void* data);
#define aura_store_add(store, key, data) _Generic((key), \
        AuraItemId: aura_store_add_with_id, \
        char*:      aura_store_add_with_str) (store, key, data)

void* aura_store_get_with_id(AuraStore* self, AuraItemId key);
void* aura_store_get_with_str(AuraStore* self, char* key);
#define aura_store_get(store, key) _Generic((key), \
        AuraItemId: aura_store_get_with_id, \
        char*:      aura_store_get_with_str) (store, key)

void* aura_store_delete_with_id(AuraStore* self, AuraItemId key);
void* aura_store_delete_with_str(AuraStore* self, char* key);
#define aura_store_delete(store, key) _Generic((key), \
        AuraItemId: aura_store_delete_with_id, \
        char*:      aura_store_delete_with_str) (store, key)

#endif // __AURA_STORE_H__

