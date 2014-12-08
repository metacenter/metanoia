// file: utils-store.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_STORE_H__
#define __AURA_STORE_H__

#include <stdint.h>

typedef uint32_t AuraItemId;

typedef struct AuraItem {
    AuraItemId id;
} AuraItem;

typedef struct AuraStore AuraStore;


static const AuraItemId scInvalidItemId = 0x0000;
static const AuraItemId scValidItemMask = 0xFFFC;


AuraStore* aura_store_new(void);

void aura_store_free(AuraStore* self);

AuraItemId aura_store_generate_new_id(AuraStore* self);

int aura_store_add(AuraStore* self, AuraItemId id, void* data);

int aura_store_delete(AuraStore* self, AuraItemId id);

void* aura_store_get(AuraStore* self, AuraItemId id);

#endif // __AURA_STORE_H__

