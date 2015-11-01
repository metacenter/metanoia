// file: tests-pool.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-pool.h"
#include "tests-suit.h"

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_when_not_resized()
{
    const unsigned initial_size = 4;
    int a[] = {1, 2, 3, 4};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));
    FILL_POOL(pool, a, int);

    ASSERT_POOL(pool, a, int, initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_when_resized_1()
{
    const unsigned initial_size = 4;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));
    FILL_POOL(pool, a, int);

    ASSERT_POOL(pool, a, int, 2*initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_when_resized_2()
{
    const unsigned initial_size = 4;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));
    FILL_POOL(pool, a, int);

    ASSERT_POOL(pool, a, int, 3*initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_add_and_get_correctly_after_realising()
{
    const unsigned initial_size = 4;
    int a1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int a2[] = {1, 2, 3, 4};

    NoiaPool* pool = noia_pool_create(initial_size, sizeof(int));

    FILL_POOL(pool, a1, int);

    noia_pool_release(pool);

    ASSERT_POOL_SIZE(pool, 0lu, 3*initial_size);

    FILL_POOL(pool, a2, int);

    ASSERT_POOL(pool, a2, int, 3*initial_size);

    noia_pool_destroy(pool);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS();

    NoiaTest test[] = {
            NOIA_TEST(should_add_and_get_correctly_when_not_resized),
            NOIA_TEST(should_add_and_get_correctly_when_resized_1),
            NOIA_TEST(should_add_and_get_correctly_when_resized_2),
            NOIA_TEST(should_add_and_get_correctly_after_realising),
        };

    return noia_test_run("Pool", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

