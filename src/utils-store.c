// file: utils-store.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-store.h"

#include <malloc.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>

struct AuraStore {
    void* root;
};

//------------------------------------------------------------------------------

static int compare(const void* data1, const void* data2)
{
    AuraItemId id1 = ((AuraItem*) data1)->id;
    AuraItemId id2 = ((AuraItem*) data2)->id;

    if (id1 < id2) return -1;
    if (id1 > id2) return  1;
    return 0;
}

//------------------------------------------------------------------------------

AuraStore* aura_store_new(void)
{
    AuraStore* self = malloc(sizeof(AuraStore));
    if (!self) {
        return NULL;
    }

    self->root = NULL;
    return self;
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
    if (self == NULL) {
        return scInvalidItemId;
    }

    AuraItem item;
    do {
        item.id = (AuraItemId) (rand() & 0xFF);
    } while (tfind((void *) &item, &self->root, compare) != NULL);
    return item.id;
}

//------------------------------------------------------------------------------

int aura_store_add(AuraStore* self, AuraItemId id, void* data)
{
    if (!self) {
        return -1;
    }

    // TODO: set id explicitly
    if (tsearch(data, &self->root, compare) == NULL) {
        return -1;
    }

    return 0;
}

//------------------------------------------------------------------------------

void* aura_store_get(AuraStore* self, AuraItemId id)
{
    if (!self) {
        return NULL;
    }

    AuraItem item;
    item.id = id;

    return *(void**) tfind((void *) &item, &self->root, compare);
}

//------------------------------------------------------------------------------

