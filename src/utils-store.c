// file: utils-store.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-store.h"

#include <malloc.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>

struct AuraStorePriv {
    void* root;
    AuraStoreCompareFunc compare;
};

//------------------------------------------------------------------------------

int aura_store_id_compare(const void* data1, const void* data2)
{
    AuraItemId id1 = ((AuraItem*) data1)->id;
    AuraItemId id2 = ((AuraItem*) data2)->id;

    if (id1 < id2) return -1;
    if (id1 > id2) return  1;
    return 0;
}

//------------------------------------------------------------------------------

int aura_store_str_compare(const void* data1, const void* data2)
{
    char* str1 = ((AuraItem*) data1)->str;
    char* str2 = ((AuraItem*) data2)->str;
    return strcmp(str1, str2);
}

//------------------------------------------------------------------------------

AuraStore* aura_store_new(AuraStoreCompareFunc compare)
{
    AuraStore* self = malloc(sizeof(AuraStore));
    if (!self) {
        return NULL;
    }

    self->root = NULL;
    self->compare = compare;
    return self;
}

//------------------------------------------------------------------------------

AuraStore* aura_store_new_for_id()
{
    return aura_store_new(aura_store_id_compare);
}

//------------------------------------------------------------------------------

AuraStore* aura_store_new_for_str()
{
    return aura_store_new(aura_store_str_compare);
}

//------------------------------------------------------------------------------

AuraStore* aura_store_new_for_data()
{
    return aura_store_new(NULL);
}

//------------------------------------------------------------------------------

void aura_store_free(AuraStore* self)
{
    if (!self) {
        return;
    }

    free(self);
}

//------------------------------------------------------------------------------

AuraItemId aura_store_generate_new_id(AuraStore* self)
{
    if (!self) {
        return scInvalidItemId;
    }

    AuraItem item;
    do {
        item.id = (AuraItemId) (rand() & scValidItemMask);
    } while (tfind((void *) &item, &self->root, self->compare) != NULL);
    return item.id;
}

//------------------------------------------------------------------------------

int aura_store_add_with_id(AuraStore* self, AuraItemId key, void* data)
{
    if (!self) {
        return -1;
    }

    AuraItem* item = (AuraItem*) data;
    item->id = key;
    if (tsearch(item, &self->root, self->compare) == NULL) {
        return -1;
    }
    return 0;
}

//------------------------------------------------------------------------------

int aura_store_add_with_str(AuraStore* self, char* key, void* data)
{
    if (!self) {
        return -1;
    }

    AuraItem* item = (AuraItem*) data;
    item->str = strdup(key);
    if (tsearch(item, &self->root, self->compare) == NULL) {
        return -1;
    }
    return 0;
}

//------------------------------------------------------------------------------

#define aura_store_get_template(KEYTYPE) \
    if (!self) { return NULL; } \
    AuraItem item; item.KEYTYPE = key; \
    void** result = tfind((void*) &item, &self->root, self->compare); \
    if (result) { return *result; } \
    return NULL;

//------------------------------------------------------------------------------

void* aura_store_get_with_id(AuraStore* self, AuraItemId key)
{
    aura_store_get_template(id);
}

//------------------------------------------------------------------------------

void* aura_store_get_with_str(AuraStore* self, char* key)
{
    aura_store_get_template(str);
}

//------------------------------------------------------------------------------

void* aura_store_delete_with_id(AuraStore* self, AuraItemId key)
{
    if (!self) {
        return NULL;
    }

    AuraItem* item = aura_store_get(self, key);
    if (tdelete(item, &self->root, self->compare) == NULL) {
        return NULL;
    }
    return item;
}

//------------------------------------------------------------------------------

void* aura_store_delete_with_str(AuraStore* self, char* key)
{
    if (!self) {
        return NULL;
    }

    AuraItem* item = aura_store_get(self, key);
    if (tdelete(item, &self->root, self->compare) == NULL) {
        return NULL;
    }

    free(item->str);
    return item;
}

//------------------------------------------------------------------------------

