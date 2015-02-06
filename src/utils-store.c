// file: utils-store.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-store.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define __USE_GNU
#define _GNU_SOURCE
#include <search.h>

struct AuraStorePriv {
    void* root;
    AuraStoreValueCompareFunc compare_value;
    AuraStoreKeyFreeFunc free_key;
    pthread_mutex_t mutex;
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

/// Tree destroy action used to free duplicated id-keys when freeing store.
void aura_store_destroy_id_key(AURA_UNUSED void* data)
{
}

//------------------------------------------------------------------------------

/// Tree destroy action used to free duplicated string-keys when freeing store.
void aura_store_destroy_string_key(void* data)
{
    AuraItem* item = (AuraItem*) data;
    if (item && item->str) {
        free(item->str);
    }
}

//------------------------------------------------------------------------------

/// Allocate memory for new AuraStore with arbitrary compare function.
AuraStore* aura_store_new(AuraStoreValueCompareFunc value_compare_func,
                          AuraStoreKeyFreeFunc key_free_func)
{
    AuraStore* self = malloc(sizeof(AuraStore));
    if (!self) {
        return NULL;
    }

    self->root = NULL;
    self->compare_value = value_compare_func;
    self->free_key = key_free_func;
    pthread_mutex_init(&self->mutex, NULL);
    return self;
}

//------------------------------------------------------------------------------

/// Allocate memory for new AuraStore that uses IDs to distinguish items.
AuraStore* aura_store_new_for_id()
{
    return aura_store_new(aura_store_id_compare, aura_store_destroy_id_key);
}

//------------------------------------------------------------------------------

/// Allocate memory for new AuraStore that uses strings to distinguish items.
AuraStore* aura_store_new_for_str()
{
    return aura_store_new(aura_store_str_compare,
                          aura_store_destroy_string_key);
}

//------------------------------------------------------------------------------

/// Free store without freeing stored items.
void aura_store_free(AuraStore* self)
{
    if (!self) {
        return;
    }

    pthread_mutex_lock(&self->mutex);
    if (self->free_key) {
        tdestroy(self->root, self->free_key);
    }
    pthread_mutex_unlock(&self->mutex);
    memset(self, 0, sizeof(AuraStore));
    free(self);
}

//------------------------------------------------------------------------------

/// Free store and stored items.
void aura_store_free_with_items(AuraStore* self, AuraFreeFunc free_func)
{
    if (!self) {
        return;
    }

    pthread_mutex_lock(&self->mutex);
    if (self->free_key) {
        tdestroy(self->root, free_func);
    }
    pthread_mutex_unlock(&self->mutex);
    memset(self, 0, sizeof(AuraStore));
    free(self);
}

//------------------------------------------------------------------------------

/// Generate new ID that is not yet present in store.
AuraItemId aura_store_generate_new_id(AuraStore* self)
{
    if (!self) {
        return scInvalidItemId;
    }

    pthread_mutex_lock(&self->mutex);
    AuraItem item;
    do {
        item.id = (AuraItemId) rand();
    } while (item.id == scInvalidItemId
          || tfind((void *) &item, &self->root, self->compare_value) != NULL);

    pthread_mutex_unlock(&self->mutex);
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

    pthread_mutex_lock(&self->mutex);
    if (tsearch(item, &self->root, self->compare_value) == NULL) {
        return AURA_RESULT_NOT_FOUND;
    }
    pthread_mutex_unlock(&self->mutex);
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

    pthread_mutex_lock(&self->mutex);
    if (tsearch(item, &self->root, self->compare_value) == NULL) {
        return AURA_RESULT_NOT_FOUND;
    }
    pthread_mutex_unlock(&self->mutex);
    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

#define aura_store_find_template(KEYTYPE) \
    if (!self) { return NULL; } \
    AuraItem item; item.KEYTYPE = key; \
    pthread_mutex_lock(&self->mutex); \
    void** result = tfind((void*) &item, &self->root, self->compare_value); \
    pthread_mutex_unlock(&self->mutex); \
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

    pthread_mutex_lock(&self->mutex);
    AuraItem* item = aura_store_find(self, key);
    if (tdelete(item, &self->root, self->compare_value) == NULL) {
        return NULL;
    }
    pthread_mutex_unlock(&self->mutex);
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

    pthread_mutex_lock(&self->mutex);
    AuraItem* item = aura_store_find(self, key);
    if (tdelete(item, &self->root, self->compare_value) == NULL) {
        return NULL;
    }
    pthread_mutex_unlock(&self->mutex);

    free(item->str);
    item->str = NULL;
    return item;
}

//------------------------------------------------------------------------------

