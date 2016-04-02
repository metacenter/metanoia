// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-pool.h"
#include "tests-suit.h"

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_when_not_resized(void)
{
    const unsigned initial_size = 4;
    int a[] = {1, 2, 3, 4};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));
    NOIA_FILL_POOL(pool, a, int);

    NOIA_ASSERT_POOL(pool, a, int, initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_when_resized_1(void)
{
    const unsigned initial_size = 4;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));
    NOIA_FILL_POOL(pool, a, int);

    NOIA_ASSERT_POOL(pool, a, int, 2*initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_when_resized_2(void)
{
    const unsigned initial_size = 4;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));
    NOIA_FILL_POOL(pool, a, int);

    NOIA_ASSERT_POOL(pool, a, int, 3*initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_after_realising(void)
{
    const unsigned initial_size = 4;
    int a1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int a2[] = {1, 2, 3, 4};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));

    NOIA_FILL_POOL(pool, a1, int);

    noia_pool_release(pool);

    NOIA_ASSERT_POOL_SIZE(pool, 0lu, 3*initial_size);

    NOIA_FILL_POOL(pool, a2, int);

    NOIA_ASSERT_POOL(pool, a2, int, 3*initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_after_dropping(void)
{
    const unsigned initial_size = 4;
    int a1 [] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int a2 [] = {1, 2, 3, 4, 5};
    int a3 [] = {11, 12, 13, 14, 15};
    int a23[] = {1, 2, 3, 4, 5, 11, 12, 13, 14, 15};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));

    NOIA_FILL_POOL(pool, a1, int);

    noia_pool_drop(pool, 5);

    NOIA_ASSERT_POOL(pool, a2, int, 3*initial_size);

    NOIA_FILL_POOL(pool, a3, int);

    NOIA_ASSERT_POOL(pool, a23, int, 3*initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should_add_and_get_correctly_when_not_resized),
            NOIA_TEST(should_add_and_get_correctly_when_resized_1),
            NOIA_TEST(should_add_and_get_correctly_when_resized_2),
            NOIA_TEST(should_add_and_get_correctly_after_realising),
            NOIA_TEST(should_add_and_get_correctly_after_dropping),
        };

    return noia_test_run("Pool", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

