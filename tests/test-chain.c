// file: tests-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-chain.h"
#include "tests-suit.h"

#include <inttypes.h>

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
                "Stored chain length should be %d (is %d)", \
                len, chain_len(CHAIN)); \
    AURA_ASSERT(len == chain_len(CHAIN), \
                "Calculated chain length should be %d (is %d)", \
                len, chain_recalculate_length(CHAIN)); \
    for (Link* link = CHAIN->first; link; link = link->next, ++i) { \
        intptr_t chain_data = (intptr_t) link->data; \
        int array_data = ARRAY[i]; \
        AURA_ASSERT(chain_data == array_data, \
                    "Chain data should be %d (is %" PRIdPTR ")", \
                    array_data, chain_data); }

//------------------------------------------------------------------------------

AuraTestResult should_adjoin_and_prejoin_links()
{
    int a[] = {4, 2, 1, 3};

    Chain* ch = chain_new((AuraFreeFunc) link_free);
    chain_adjoin (ch, link_new((void*) 1));
    chain_prejoin(ch, link_new((void*) 2));
    chain_adjoin (ch, link_new((void*) 3));
    chain_prejoin(ch, link_new((void*) 4));

    ASSERT_CHAIN(ch, a);
    chain_free(ch);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_unjoin_from_begining()
{
    int a[] = {2, 3};

    Chain* ch = chain_new((AuraFreeFunc) link_free);
    chain_adjoin(ch, link_new((void*) 1));
    chain_adjoin(ch, link_new((void*) 2));
    chain_adjoin(ch, link_new((void*) 3));

    Link* link = ch->first;
    chain_unjoin(ch, link);

    ASSERT_CHAIN(ch, a);
    link_free(link);
    chain_free(ch);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_unjoin_from_end()
{
    int a[] = {1, 2};

    Chain* ch = chain_new((AuraFreeFunc) link_free);
    chain_adjoin(ch, link_new((void*) 1));
    chain_adjoin(ch, link_new((void*) 2));
    chain_adjoin(ch, link_new((void*) 3));

    Link* link = ch->last;
    chain_unjoin(ch, link);

    ASSERT_CHAIN(ch, a);
    link_free(link);
    chain_free(ch);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_unjoin_from_inside()
{
    int a[] = {1, 3};

    Chain* ch = chain_new((AuraFreeFunc) link_free);
    chain_adjoin(ch, link_new((void*) 1));
    chain_adjoin(ch, link_new((void*) 2));
    chain_adjoin(ch, link_new((void*) 3));

    Link* link = ch->first->next;
    chain_unjoin(ch, ch->first->next);

    ASSERT_CHAIN(ch, a);
    link_free(link);
    chain_free(ch);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_rejoin_without_cicles()
{
    int a[] = {2, 1};

    Chain* ch = chain_new((AuraFreeFunc) link_free);
    chain_adjoin(ch, link_new((void*) 1));
    chain_adjoin(ch, link_new((void*) 2));

    Link* link = ch->first;
    chain_unjoin(ch, link);
    chain_adjoin(ch, link);

    ASSERT_CHAIN(ch, a);
    chain_free(ch);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    AuraTest test[] = {
            AURA_TEST(should_adjoin_and_prejoin_links),
            AURA_TEST(should_unjoin_from_begining),
            AURA_TEST(should_unjoin_from_end),
            AURA_TEST(should_unjoin_from_inside),
            AURA_TEST(should_rejoin_without_cicles)
        };

    return aura_test_run("Chain", test, AURA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

