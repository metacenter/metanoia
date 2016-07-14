// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-store.h"
#include "tests-suit.h"
#include <malloc.h>

typedef struct Elem {
    NoiaItem base;
    int value;
} Elem;

//------------------------------------------------------------------------------

NoiaTestResult should_correctly_store_data_for_id(void)
{
    NoiaItemId key1 = 4;
    NoiaItemId key2 = 7;
    NoiaItemId key3 = 9;
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.base.id = key1;
    e1.value = value1;

    Elem e2;
    e2.base.id = key3;
    e2.value = value2;

    NoiaStore* store = noia_store_new_for_id();
    noia_store_add(store, key1, &e1);
    noia_store_add(store, key2, &e2);

    Elem* r1 = noia_store_find(store, key1);
    NOIA_ASSERT(r1, "r1 should be stored")
    NOIA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = noia_store_find(store, key3);
    NOIA_ASSERT(!r2, "r2 should not be stored")

    Elem* r3 = noia_store_find(store, key2);
    NOIA_ASSERT(r3, "r3 should be stored")
    NOIA_ASSERT(r3->value == value2,
                "r3 value should be %d (is %d)",
                value2, r3->value)

    noia_store_free(store);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_correctly_delete_data_for_id(void)
{
    NoiaItemId key1 = 4;
    NoiaItemId key2 = 7;
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.base.id = key1;
    e1.value = value1;

    Elem e2;
    e2.base.id = key2;
    e2.value = value2;

    NoiaStore* store = noia_store_new_for_id();
    noia_store_add(store, key1, &e1);
    noia_store_add(store, key2, &e2);
    Elem* rd = noia_store_delete(store, key2);

    NOIA_ASSERT(rd, "rd should be non-null")
    NOIA_ASSERT(rd->value == value2,
                "rd value should be %d (is %d)",
                value2, rd->value)

    Elem* r1 = noia_store_find(store, key1);
    NOIA_ASSERT(r1, "r1 should be stored")
    NOIA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = noia_store_find(store, key2);
    NOIA_ASSERT(!r2, "r2 should not be stored")

    noia_store_free(store);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_correctly_store_data_for_str(void)
{
    const char* key1 = "key_str_1";
    const char* key2 = "key_str_2";
    const char* key3 = "key_str_3";
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.value = value1;

    Elem e2;
    e2.value = value2;

    NoiaStore* store = noia_store_new_for_str();
    noia_store_add(store, key1, &e1);
    noia_store_add(store, key2, &e2);

    Elem* r1 = noia_store_find(store, key1);
    NOIA_ASSERT(r1, "r1 should be stored")
    NOIA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = noia_store_find(store, key3);
    NOIA_ASSERT(!r2, "r2 should not be stored")

    Elem* r3 = noia_store_find(store, key2);
    NOIA_ASSERT(r3, "r3 should be stored")
    NOIA_ASSERT(r3->value == value2,
                "r3 value should be %d (is %d)",
                value2, r3->value)

    noia_store_free(store);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_correctly_delete_data_for_str(void)
{
    const char* key1 = "key_str_1";
    const char* key2 = "key_str_2";
    int value1 = 5;
    int value2 = 8;

    Elem e1;
    e1.value = value1;

    Elem e2;
    e2.value = value2;

    NoiaStore* store = noia_store_new_for_str();
    noia_store_add(store, key1, &e1);
    noia_store_add(store, key2, &e2);
    Elem* rd = noia_store_delete(store, key2);

    NOIA_ASSERT(rd, "rd should be non-null")
    NOIA_ASSERT(rd->value == value2,
                "rd value should be %d (is %d)",
                value2, rd->value)

    Elem* r1 = noia_store_find(store, key1);
    NOIA_ASSERT(r1, "r1 should be stored")
    NOIA_ASSERT(r1->value == value1,
                "r1 value should be %d (is %d)",
                value1, r1->value)

    Elem* r2 = noia_store_find(store, key2);
    NOIA_ASSERT(!r2, "r2 should not be stored")

    noia_store_free(store);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should_correctly_store_data_for_id),
            NOIA_TEST(should_correctly_delete_data_for_id),
            NOIA_TEST(should_correctly_store_data_for_str),
            NOIA_TEST(should_correctly_delete_data_for_str)
        };

    return noia_test_run("Store", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

