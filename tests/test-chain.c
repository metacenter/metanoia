// file: tests-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-chain.h"
#include "tests-suit.h"

#include <malloc.h>

//------------------------------------------------------------------------------

void noia_test_free_link_with_str(void* data)
{
    link_destroy((Link*) data, free);
}

//------------------------------------------------------------------------------

NoiaTestResult should_adjoin_and_prejoin_links()
{
    char* a[] = {"4", "2", "1", "3"};

    Chain* ch = chain_new(noia_test_free_link_with_str);
    chain_adjoin (ch, link_new(strdup("1")));
    chain_prejoin(ch, link_new(strdup("2")));
    chain_adjoin (ch, link_new(strdup("3")));
    chain_prejoin(ch, link_new(strdup("4")));

    ASSERT_CHAIN(ch, a);
    chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_unjoin_from_begining()
{
    char* a[] = {"2", "3"};

    Chain* ch = chain_new(noia_test_free_link_with_str);
    chain_adjoin(ch, link_new(strdup("1")));
    chain_adjoin(ch, link_new(strdup("2")));
    chain_adjoin(ch, link_new(strdup("3")));

    Link* link = ch->first;
    chain_unjoin(ch, link);

    ASSERT_CHAIN(ch, a);
    link_destroy(link, free);
    chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_unjoin_from_end()
{
    char* a[] = {"1", "2"};

    Chain* ch = chain_new(noia_test_free_link_with_str);
    chain_adjoin(ch, link_new(strdup("1")));
    chain_adjoin(ch, link_new(strdup("2")));
    chain_adjoin(ch, link_new(strdup("3")));

    Link* link = ch->last;
    chain_unjoin(ch, link);

    ASSERT_CHAIN(ch, a);
    link_destroy(link, free);
    chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_unjoin_from_inside()
{
    char* a[] = {"1", "3"};

    Chain* ch = chain_new(noia_test_free_link_with_str);
    chain_adjoin(ch, link_new(strdup("1")));
    chain_adjoin(ch, link_new(strdup("2")));
    chain_adjoin(ch, link_new(strdup("3")));

    Link* link = ch->first->next;
    chain_unjoin(ch, ch->first->next);

    ASSERT_CHAIN(ch, a);
    link_destroy(link, free);
    chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_rejoin_without_cicles()
{
    char* a[] = {"2", "1"};

    Chain* ch = chain_new(noia_test_free_link_with_str);
    chain_adjoin(ch, link_new(strdup("1")));
    chain_adjoin(ch, link_new(strdup("2")));

    Link* link = ch->first;
    chain_unjoin(ch, link);
    chain_adjoin(ch, link);

    ASSERT_CHAIN(ch, a);
    chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    NoiaTest test[] = {
            NOIA_TEST(should_adjoin_and_prejoin_links),
            NOIA_TEST(should_unjoin_from_begining),
            NOIA_TEST(should_unjoin_from_end),
            NOIA_TEST(should_unjoin_from_inside),
            NOIA_TEST(should_rejoin_without_cicles)
        };

    return noia_test_run("Chain", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

