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

struct NoiaStorePriv {
    void* root;
    NoiaStoreValueCompareFunc compare_value;
    NoiaStoreKeyFreeFunc free_key;
    pthread_mutex_t mutex;
};

//------------------------------------------------------------------------------

/// Compare two NoiaItems using identifies.
int noia_store_id_compare(const void* data1, const void* data2)
{
    NoiaItemId id1 = ((NoiaItem*) data1)->id;
    NoiaItemId id2 = ((NoiaItem*) data2)->id;

    if (id1 < id2) return -1;
    if (id1 > id2) return  1;
    return 0;
}

//------------------------------------------------------------------------------

/// Compare two NoiaItems using strings.
int noia_store_str_compare(const void* data1, const void* data2)
{
    char* str1 = ((NoiaItem*) data1)->str;
    char* str2 = ((NoiaItem*) data2)->str;
    return strcmp(str1, str2);
}

//------------------------------------------------------------------------------

/// Tree destroy action used to free duplicated id-keys when freeing store.
void noia_store_destroy_id_key(NOIA_UNUSED void* data)
{
}

//------------------------------------------------------------------------------

/// Tree destroy action used to free duplicated string-keys when freeing store.
void noia_store_destroy_string_key(void* data)
{
    NoiaItem* item = (NoiaItem*) data;
    if (item && item->str) {
        free(item->str);
    }
}

//------------------------------------------------------------------------------

/// Allocate memory for new NoiaStore with arbitrary compare function.
NoiaStore* noia_store_new(NoiaStoreValueCompareFunc value_compare_func,
                          NoiaStoreKeyFreeFunc key_free_func)
{
    NoiaStore* self = malloc(sizeof(NoiaStore));
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

/// Allocate memory for new NoiaStore that uses IDs to distinguish items.
NoiaStore* noia_store_new_for_id()
{
    return noia_store_new(noia_store_id_compare, noia_store_destroy_id_key);
}

//------------------------------------------------------------------------------

/// Allocate memory for new NoiaStore that uses strings to distinguish items.
NoiaStore* noia_store_new_for_str()
{
    return noia_store_new(noia_store_str_compare,
                          noia_store_destroy_string_key);
}

//------------------------------------------------------------------------------

/// Free store without freeing stored items.
void noia_store_free(NoiaStore* self)
{
    if (!self) {
        return;
    }

    pthread_mutex_lock(&self->mutex);
    if (self->free_key) {
        tdestroy(self->root, self->free_key);
    }
    pthread_mutex_unlock(&self->mutex);
    memset(self, 0, sizeof(NoiaStore));
    free(self);
}

//------------------------------------------------------------------------------

#include "utils-log.h"

/// Free store and stored items.
void noia_store_free_with_items(NoiaStore* self, NoiaFreeFunc free_func)
{
    if (!self) {
        return;
    }

    pthread_mutex_lock(&self->mutex);
    if (self->free_key) {
        tdestroy(self->root, free_func);
    }
    pthread_mutex_unlock(&self->mutex);
    memset(self, 0, sizeof(NoiaStore));
    free(self);
}

//------------------------------------------------------------------------------

/// Generate new ID that is not yet present in store.
NoiaItemId noia_store_generate_new_id(NoiaStore* self)
{
    if (!self) {
        return scInvalidItemId;
    }

    pthread_mutex_lock(&self->mutex);
    NoiaItem item;
    do {
        item.id = (NoiaItemId) rand();
    } while (item.id == scInvalidItemId
          || tfind((void *) &item, &self->root, self->compare_value) != NULL);

    pthread_mutex_unlock(&self->mutex);
    return item.id;
}

//------------------------------------------------------------------------------

/// Store item using ID.
/// @param key - ID used as a key
/// @param data - item to be stored
NoiaResult noia_store_add_with_id(NoiaStore* self, NoiaItemId key, void* data)
{
    if (!self) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    NoiaItem* item = (NoiaItem*) data;
    item->id = key;

    pthread_mutex_lock(&self->mutex);
    if (tsearch(item, &self->root, self->compare_value) == NULL) {
        return NOIA_RESULT_NOT_FOUND;
    }
    pthread_mutex_unlock(&self->mutex);
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Store item using string.
/// The string used as a key is duplicated.
/// @param key - string used as a key
/// @param data - item to be stored
NoiaResult noia_store_add_with_str(NoiaStore* self, char* key, void* data)
{
    if (!self) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    NoiaItem* item = (NoiaItem*) data;
    item->str = strdup(key);

    pthread_mutex_lock(&self->mutex);
    if (tsearch(item, &self->root, self->compare_value) == NULL) {
        return NOIA_RESULT_NOT_FOUND;
    }
    pthread_mutex_unlock(&self->mutex);
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

#define noia_store_find_template(KEYTYPE) \
    if (!self) { return NULL; } \
    NoiaItem item; item.KEYTYPE = key; \
    pthread_mutex_lock(&self->mutex); \
    void** result = tfind((void*) &item, &self->root, self->compare_value); \
    pthread_mutex_unlock(&self->mutex); \
    if (result) { return *result; } \
    return NULL;

//------------------------------------------------------------------------------

/// Store item using ID.
/// @param key - ID used to reference an item
/// @return pointer to found item or null if nothing found
void* noia_store_find_with_id(NoiaStore* self, NoiaItemId key)
{
    noia_store_find_template(id);
}

//------------------------------------------------------------------------------

/// Store item using string.
/// @param key - string used to reference an item
/// @return pointer to found item or null if nothing found
void* noia_store_find_with_str(NoiaStore* self, char* key)
{
    noia_store_find_template(str);
}

//------------------------------------------------------------------------------

/// Delete an item using ID.
/// @param key - ID used to reference an item
/// @return pointer to found item or null if nothing found
void* noia_store_delete_with_id(NoiaStore* self, NoiaItemId key)
{
    if (!self) {
        return NULL;
    }

    NoiaItem* item = noia_store_find(self, key);
    pthread_mutex_lock(&self->mutex);
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
void* noia_store_delete_with_str(NoiaStore* self, char* key)
{
    if (!self) {
        return NULL;
    }

    NoiaItem* item = noia_store_find(self, key);
    pthread_mutex_lock(&self->mutex);
    if (tdelete(item, &self->root, self->compare_value) == NULL) {
        return NULL;
    }
    pthread_mutex_unlock(&self->mutex);

    free(item->str);
    item->str = NULL;
    return item;
}

//------------------------------------------------------------------------------

