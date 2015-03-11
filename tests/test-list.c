// file: tests-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-list.h"
#include "tests-suit.h"

#include <malloc.h>

//------------------------------------------------------------------------------

NoiaTestResult should_append_values()
{
    char* a[] = {"1", "2", "3"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));

    ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_prepend_values()
{
    char* a[] = {"3", "2", "1"};

    NoiaList* l = noia_list_new(free);
    noia_list_prepend(l, strdup("1"));
    noia_list_prepend(l, strdup("2"));
    noia_list_prepend(l, strdup("3"));

    ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_from_begining()
{
    char* a[] = {"2", "3"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));

    noia_list_remove(l, "1", (NoiaCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_from_end()
{
    char* a[] = {"1", "2"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));

    noia_list_remove(l, "3", (NoiaCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_from_inside()
{
    char* a[] = {"1", "3"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));

    noia_list_remove(l, "2", (NoiaCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_all_equal()
{
    char* a[] = {"1", "3"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));
    noia_list_append(l, strdup("2"));

    noia_list_remove_all(l, "2", (NoiaCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    NoiaTest test[] = {
            NOIA_TEST(should_append_values),
            NOIA_TEST(should_prepend_values),
            NOIA_TEST(should_remove_from_begining),
            NOIA_TEST(should_remove_from_end),
            NOIA_TEST(should_remove_from_inside),
            NOIA_TEST(should_remove_all_equal),
        };

    return noia_test_run("List", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

