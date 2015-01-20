// file: tests-store.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-chain.h"
#include "tests-suit.h"

#define ARRAY_LEN(a,t) (sizeof(a)/sizeof(t))

int compare(int i1, int i2) {
    if (i1 < i2) {
        return -1;
    } else if (i1 > i2) {
        return 1;
    }
    return 0;
}

#define ASSERT_CHAIN(CHAIN, ARRAY) \
    int i = 0, len = ARRAY_LEN(ARRAY, int); \
    AURA_ASSERT(len == chain_len(CHAIN), \
                "Chain length should be %d (is %d)", len, chain_len(CHAIN)); \
    for (Link* link = CHAIN->first; link; link = link->next, ++i) { \
        int chain_data = (int) link->data; \
        int array_data = ARRAY[i]; \
        AURA_ASSERT(chain_data == array_data, \
                    "Chain data should be %d (is %d)", \
                    array_data, chain_data); }

//------------------------------------------------------------------------------

AuraTestResult should_append_values()
{
    int a[] = {1, 2, 3};

    Chain* ch = chain_new(0);
    chain_append(ch, (void*) 1);
    chain_append(ch, (void*) 2);
    chain_append(ch, (void*) 3);

    ASSERT_CHAIN(ch, a);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_prepend_values()
{
    int a[] = {3, 2, 1};

    Chain* ch = chain_new(0);
    chain_prepend(ch, (void*) 1);
    chain_prepend(ch, (void*) 2);
    chain_prepend(ch, (void*) 3);

    ASSERT_CHAIN(ch, a);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_adjoin_and_prejoin_links()
{
    int a[] = {4, 2, 1, 3};

    Chain* ch = chain_new(0);
    chain_adjoin (ch, link_new((void*) 1));
    chain_prejoin(ch, link_new((void*) 2));
    chain_adjoin (ch, link_new((void*) 3));
    chain_prejoin(ch, link_new((void*) 4));

    ASSERT_CHAIN(ch, a);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_begining()
{
    int a[] = {2, 3};

    Chain* ch = chain_new(0);
    chain_append(ch, (void*) 1);
    chain_append(ch, (void*) 2);
    chain_append(ch, (void*) 3);

    chain_remove(ch, (void*) 1, (AuraCompareFunc) compare);

    ASSERT_CHAIN(ch, a);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_end()
{
    int a[] = {1, 2};

    Chain* ch = chain_new(0);
    chain_append(ch, (void*) 1);
    chain_append(ch, (void*) 2);
    chain_append(ch, (void*) 3);

    chain_remove(ch, (void*) 3, (AuraCompareFunc) compare);

    ASSERT_CHAIN(ch, a);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_inside()
{
    int a[] = {1, 3};

    Chain* ch = chain_new(0);
    chain_append(ch, (void*) 1);
    chain_append(ch, (void*) 2);
    chain_append(ch, (void*) 3);

    chain_remove(ch, (void*) 2, (AuraCompareFunc) compare);

    ASSERT_CHAIN(ch, a);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    AuraTest test[] = {
            AURA_TEST(should_append_values),
            AURA_TEST(should_prepend_values),
            AURA_TEST(should_adjoin_and_prejoin_links),
            AURA_TEST(should_remove_from_begining),
            AURA_TEST(should_remove_from_end),
            AURA_TEST(should_remove_from_inside)
        };

    return aura_test_run("Chain", test, AURA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

