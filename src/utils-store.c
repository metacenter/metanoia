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

/// Compare two AuraItems using identifies.
int aura_store_id_compare(const void* data1, const void* data2)
{
    AuraItemId id1 = ((AuraItem*) data1)->id;
    AuraItemId id2 = ((AuraItem*) data2)->id;

    if (id1 < id2) return -1;
    if (id1 > id2) return  1;
    return 0;
}

//------------------------------------------------------------------------------

/// Compare two AuraItems using strings.
int aura_store_str_compare(const void* data1, const void* data2)
{
    char* str1 = ((AuraItem*) data1)->str;
    char* str2 = ((AuraItem*) data2)->str;
    return strcmp(str1, str2);
}

//------------------------------------------------------------------------------

/// Allocate memory for new AuraStore with arbitrary compare function.
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

/// Allocate memory for new AuraStore that uses IDs to distinguish items.
AuraStore* aura_store_new_for_id()
{
    return aura_store_new(aura_store_id_compare);
}

//------------------------------------------------------------------------------

/// Allocate memory for new AuraStore that uses strings to distinguish items.
AuraStore* aura_store_new_for_str()
{
    return aura_store_new(aura_store_str_compare);
}

//------------------------------------------------------------------------------

/// Free store (do not free stored items).
void aura_store_free(AuraStore* self)
{
    if (!self) {
        return;
    }

    free(self);
}

//------------------------------------------------------------------------------

/// Generate new ID that is not yet present in store.
AuraItemId aura_store_generate_new_id(AuraStore* self)
{
    if (!self) {
        return scInvalidItemId;
    }

    AuraItem item;
    do {
        item.id = (AuraItemId) rand();
    } while (item.id == scInvalidItemId
          || tfind((void *) &item, &self->root, self->compare) != NULL);

    return item.id;
}

//------------------------------------------------------------------------------

/// Store item using ID.
/// @param key - ID used as a key
/// @param data - item to be stored
AuraResult aura_store_add_with_id(AuraStore* self, AuraItemId key, void* data)
{
    if (!self) {
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    AuraItem* item = (AuraItem*) data;
    item->id = key;
    if (tsearch(item, &self->root, self->compare) == NULL) {
        return AURA_RESULT_NOT_FOUND;
    }
    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Store item using string.
/// The string used as a key is duplicated.
/// @param key - string used as a key
/// @param data - item to be stored
AuraResult aura_store_add_with_str(AuraStore* self, char* key, void* data)
{
    if (!self) {
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    AuraItem* item = (AuraItem*) data;
    item->str = strdup(key);
    if (tsearch(item, &self->root, self->compare) == NULL) {
        return AURA_RESULT_NOT_FOUND;
    }
    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

#define aura_store_find_template(KEYTYPE) \
    if (!self) { return NULL; } \
    AuraItem item; item.KEYTYPE = key; \
    void** result = tfind((void*) &item, &self->root, self->compare); \
    if (result) { return *result; } \
    return NULL;

//------------------------------------------------------------------------------

/// Store item using ID.
/// @param key - ID used to reference an item
/// @return pointer to found item or null if nothing found
void* aura_store_find_with_id(AuraStore* self, AuraItemId key)
{
    aura_store_find_template(id);
}

//------------------------------------------------------------------------------

/// Store item using string.
/// @param key - string used to reference an item
/// @return pointer to found item or null if nothing found
void* aura_store_find_with_str(AuraStore* self, char* key)
{
    aura_store_find_template(str);
}

//------------------------------------------------------------------------------

/// Delete an item using ID.
/// @param key - ID used to reference an item
/// @return pointer to found item or null if nothing found
void* aura_store_delete_with_id(AuraStore* self, AuraItemId key)
{
    if (!self) {
        return NULL;
    }

    AuraItem* item = aura_store_find(self, key);
    if (tdelete(item, &self->root, self->compare) == NULL) {
        return NULL;
    }
    return item;
}

//------------------------------------------------------------------------------

/// Delete an item using string.
/// @param key - string used to reference an item
/// @return pointer to found item or null if nothing found
void* aura_store_delete_with_str(AuraStore* self, char* key)
{
    if (!self) {
        return NULL;
    }

    AuraItem* item = aura_store_find(self, key);
    if (tdelete(item, &self->root, self->compare) == NULL) {
        return NULL;
    }

    free(item->str);
    return item;
}

//------------------------------------------------------------------------------

