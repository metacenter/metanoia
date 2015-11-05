// file: tests-chain.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-chain.h"
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
    char* a[] = {"3", "4"};

    Chain* ch = chain_new(noia_test_free_link_with_str);
    chain_adjoin(ch, link_new(strdup("1")));
    chain_adjoin(ch, link_new(strdup("2")));
    chain_adjoin(ch, link_new(strdup("3")));
    chain_adjoin(ch, link_new(strdup("4")));

    Link* link1 = ch->first;
    chain_unjoin(ch, link1);

    Link* link2 = ch->first;
    chain_unjoin(ch, link2);

    ASSERT_CHAIN(ch, a);
    link_destroy(link1, free);
    link_destroy(link2, free);
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
    chain_adjoin(ch, link_new(strdup("4")));

    Link* link1 = ch->last;
    chain_unjoin(ch, link1);

    Link* link2 = ch->last;
    chain_unjoin(ch, link2);

    ASSERT_CHAIN(ch, a);
    link_destroy(link1, free);
    link_destroy(link2, free);
    chain_free(ch);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_unjoin_from_inside()
{
    char* a[] = {"1", "4"};

    Chain* ch = chain_new(noia_test_free_link_with_str);
    chain_adjoin(ch, link_new(strdup("1")));
    chain_adjoin(ch, link_new(strdup("2")));
    chain_adjoin(ch, link_new(strdup("3")));
    chain_adjoin(ch, link_new(strdup("4")));

    Link* link1 = ch->first->next;
    chain_unjoin(ch, link1);

    Link* link2 = ch->first->next;
    chain_unjoin(ch, link2);

    ASSERT_CHAIN(ch, a);
    link_destroy(link1, free);
    link_destroy(link2, free);
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

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS();

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

