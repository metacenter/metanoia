// file: tests-store.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-store.h"
#include "tests-suit.h"
#include <malloc.h>

typedef struct Elem {
    AuraItem base;
    int value;
} Elem;

//------------------------------------------------------------------------------

AuraTestResult should_correctly_store_data_for_id()
{
    AuraItemId key1 = 4;
    AuraItemId key2 = 7;
    AuraItemId key3 = 9;
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.base.id = key1;
    e1.value = value1;

    Elem e2;
    e2.base.id = key3;
    e2.value = value2;

    AuraStore* store = aura_store_new_for_id();
    aura_store_add(store, key1, &e1);
    aura_store_add(store, key2, &e2);

    Elem* r1 = aura_store_find(store, key1);
    AURA_ASSERT(r1, "r1 should be stored")
    AURA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = aura_store_find(store, key3);
    AURA_ASSERT(!r2, "r2 should not be stored")

    Elem* r3 = aura_store_find(store, key2);
    AURA_ASSERT(r3, "r3 should be stored")
    AURA_ASSERT(r3->value == value2,
                "r3 value should be %d (is %d)",
                value2, r3->value)

    aura_store_free(store);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_correctly_delete_data_for_id()
{
    AuraItemId key1 = 4;
    AuraItemId key2 = 7;
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.base.id = key1;
    e1.value = value1;

    Elem e2;
    e2.base.id = key2;
    e2.value = value2;

    AuraStore* store = aura_store_new_for_id();
    aura_store_add(store, key1, &e1);
    aura_store_add(store, key2, &e2);
    Elem* rd = aura_store_delete(store, key2);

    AURA_ASSERT(rd, "rd should be non-null")
    AURA_ASSERT(rd->value == value2,
                "rd value should be %d (is %d)",
                value2, rd->value)

    Elem* r1 = aura_store_find(store, key1);
    AURA_ASSERT(r1, "r1 should be stored")
    AURA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = aura_store_find(store, key2);
    AURA_ASSERT(!r2, "r2 should not be stored")

    aura_store_free(store);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_correctly_store_data_for_str()
{
    char* key1 = "key_str_1";
    char* key2 = "key_str_2";
    char* key3 = "key_str_3";
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.base.str = key1;
    e1.value = value1;

    Elem e2;
    e2.base.str = key3;
    e2.value = value2;

    AuraStore* store = aura_store_new_for_str();
    aura_store_add(store, key1, &e1);
    aura_store_add(store, key2, &e2);

    Elem* r1 = aura_store_find(store, key1);
    AURA_ASSERT(r1, "r1 should be stored")
    AURA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = aura_store_find(store, key3);
    AURA_ASSERT(!r2, "r2 should not be stored")

    Elem* r3 = aura_store_find(store, key2);
    AURA_ASSERT(r3, "r3 should be stored")
    AURA_ASSERT(r3->value == value2,
                "r3 value should be %d (is %d)",
                value2, r3->value)

    aura_store_free(store);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_correctly_delete_data_for_str()
{
    char* key1 = "key_str_1";
    char* key2 = "key_str_2";
    char* key1_copy = strdup(key1);
    char* key2_copy = strdup(key2);
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.value = value1;

    Elem e2;
    e2.value = value2;

    AuraStore* store = aura_store_new_for_str();
    aura_store_add(store, key1_copy, &e1);
    aura_store_add(store, key2_copy, &e2);
    free(key1_copy);
    free(key2_copy);
    Elem* rd = aura_store_delete(store, key2);

    AURA_ASSERT(rd, "rd should be non-null")
    AURA_ASSERT(rd->value == value2,
                "rd value should be %d (is %d)",
                value2, rd->value)

    Elem* r1 = aura_store_find(store, key1);
    AURA_ASSERT(r1, "r1 should be stored")
    AURA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = aura_store_find(store, key2);
    AURA_ASSERT(!r2, "r2 should not be stored")

    aura_store_free(store);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    AuraTest test[] = {
            AURA_TEST(should_correctly_store_data_for_id),
            AURA_TEST(should_correctly_delete_data_for_id),
            AURA_TEST(should_correctly_store_data_for_str),
            AURA_TEST(should_correctly_delete_data_for_str)
        };

    return aura_test_run("Store", test, AURA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

