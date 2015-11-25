// file: tests-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-chain.h"
#include "tests-suit.h"

#include <malloc.h>

//------------------------------------------------------------------------------

void noia_test_free_link_with_str(void* data)
{
    noia_link_destroy((NoiaLink*) data, free);
}

//------------------------------------------------------------------------------

NoiaTestResult should_adjoin_and_prejoin_links()
{
    char* a[] = {"4", "2", "1", "3"};

    NoiaChain* ch = noia_chain_new(noia_test_free_link_with_str);
    noia_chain_adjoin (ch, noia_link_new(strdup("1")));
    noia_chain_prejoin(ch, noia_link_new(strdup("2")));
    noia_chain_adjoin (ch, noia_link_new(strdup("3")));
    noia_chain_prejoin(ch, noia_link_new(strdup("4")));

    ASSERT_CHAIN(ch, a);
    noia_chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_prejoin_links_onto()
{
    char* a[] = {"1", "2", "3", "4", "5"};

    NoiaChain* ch = noia_chain_new(noia_test_free_link_with_str);
    NoiaLink* link1 = noia_link_new(strdup("1"));
    NoiaLink* link2 = noia_link_new(strdup("2"));
    NoiaLink* link3 = noia_link_new(strdup("3"));
    NoiaLink* link4 = noia_link_new(strdup("4"));
    NoiaLink* link5 = noia_link_new(strdup("5"));

    noia_chain_prejoin_onto(ch, link5, NULL);
    noia_chain_prejoin_onto(ch, link2, link5);
    noia_chain_prejoin_onto(ch, link1, link2);
    noia_chain_prejoin_onto(ch, link4, link5);
    noia_chain_prejoin_onto(ch, link3, link4);

    ASSERT_CHAIN(ch, a);
    noia_chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_adjoin_links_onto()
{
    char* a[] = {"1", "2", "3", "4", "5"};

    NoiaChain* ch = noia_chain_new(noia_test_free_link_with_str);
    NoiaLink* link1 = noia_link_new(strdup("1"));
    NoiaLink* link2 = noia_link_new(strdup("2"));
    NoiaLink* link3 = noia_link_new(strdup("3"));
    NoiaLink* link4 = noia_link_new(strdup("4"));
    NoiaLink* link5 = noia_link_new(strdup("5"));

    noia_chain_adjoin_onto(ch, link1, NULL);
    noia_chain_adjoin_onto(ch, link4, link1);
    noia_chain_adjoin_onto(ch, link5, link4);
    noia_chain_adjoin_onto(ch, link2, link1);
    noia_chain_adjoin_onto(ch, link3, link2);

    ASSERT_CHAIN(ch, a);
    noia_chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_unjoin_from_begining()
{
    char* a[] = {"3", "4"};

    NoiaChain* ch = noia_chain_new(noia_test_free_link_with_str);
    noia_chain_adjoin(ch, noia_link_new(strdup("1")));
    noia_chain_adjoin(ch, noia_link_new(strdup("2")));
    noia_chain_adjoin(ch, noia_link_new(strdup("3")));
    noia_chain_adjoin(ch, noia_link_new(strdup("4")));

    NoiaLink* link1 = ch->first;
    noia_chain_unjoin(ch, link1);

    NoiaLink* link2 = ch->first;
    noia_chain_unjoin(ch, link2);

    ASSERT_CHAIN(ch, a);
    noia_link_destroy(link1, free);
    noia_link_destroy(link2, free);
    noia_chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_unjoin_from_end()
{
    char* a[] = {"1", "2"};

    NoiaChain* ch = noia_chain_new(noia_test_free_link_with_str);
    noia_chain_adjoin(ch, noia_link_new(strdup("1")));
    noia_chain_adjoin(ch, noia_link_new(strdup("2")));
    noia_chain_adjoin(ch, noia_link_new(strdup("3")));
    noia_chain_adjoin(ch, noia_link_new(strdup("4")));

    NoiaLink* link1 = ch->last;
    noia_chain_unjoin(ch, link1);

    NoiaLink* link2 = ch->last;
    noia_chain_unjoin(ch, link2);

    ASSERT_CHAIN(ch, a);
    noia_link_destroy(link1, free);
    noia_link_destroy(link2, free);
    noia_chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_unjoin_from_inside()
{
    char* a[] = {"1", "4"};

    NoiaChain* ch = noia_chain_new(noia_test_free_link_with_str);
    noia_chain_adjoin(ch, noia_link_new(strdup("1")));
    noia_chain_adjoin(ch, noia_link_new(strdup("2")));
    noia_chain_adjoin(ch, noia_link_new(strdup("3")));
    noia_chain_adjoin(ch, noia_link_new(strdup("4")));

    NoiaLink* link1 = ch->first->next;
    noia_chain_unjoin(ch, link1);

    NoiaLink* link2 = ch->first->next;
    noia_chain_unjoin(ch, link2);

    ASSERT_CHAIN(ch, a);
    noia_link_destroy(link1, free);
    noia_link_destroy(link2, free);
    noia_chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_rejoin_without_cicles()
{
    char* a[] = {"2", "1"};

    NoiaChain* ch = noia_chain_new(noia_test_free_link_with_str);
    noia_chain_adjoin(ch, noia_link_new(strdup("1")));
    noia_chain_adjoin(ch, noia_link_new(strdup("2")));

    NoiaLink* link = ch->first;
    noia_chain_unjoin(ch, link);
    noia_chain_adjoin(ch, link);

    ASSERT_CHAIN(ch, a);
    noia_chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS();

    NoiaTest test[] = {
            NOIA_TEST(should_adjoin_and_prejoin_links),
            NOIA_TEST(should_prejoin_links_onto),
            NOIA_TEST(should_adjoin_links_onto),
            NOIA_TEST(should_unjoin_from_begining),
            NOIA_TEST(should_unjoin_from_end),
            NOIA_TEST(should_unjoin_from_inside),
            NOIA_TEST(should_rejoin_without_cicles)
        };

    return noia_test_run("Chain", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

