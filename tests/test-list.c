// file: tests-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-list.h"
#include "tests-suit.h"

#include <malloc.h>

//------------------------------------------------------------------------------

NoiaTestResult should_append_values(void)
{
    char* a[] = {"1", "2", "3"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));

    NOIA_ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_prepend_values(void)
{
    char* a[] = {"3", "2", "1"};

    NoiaList* l = noia_list_new(free);
    noia_list_prepend(l, strdup("1"));
    noia_list_prepend(l, strdup("2"));
    noia_list_prepend(l, strdup("3"));

    NOIA_ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_get_nth(void)
{
    char* e;
    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("0"));
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));
    noia_list_append(l, strdup("4"));

    e = noia_list_get_nth(l, 0);
    NOIA_ASSERT(strcmp("0", e) == 0,
               "First element should be '0' (is '%s')", e);

    e = noia_list_get_nth(l, 1);
    NOIA_ASSERT(strcmp("1", e) == 0,
               "Second element should be '1' (is '%s')", e);

    e = noia_list_get_nth(l, 4);
    NOIA_ASSERT(strcmp("4", e) == 0,
               "Last element should be '4' (is '%s')", e);

    e = noia_list_get_nth(l, -1);
    NOIA_ASSERT(strcmp("4", e) == 0,
               "Last element should be '4' (is '%s')", e);

    e = noia_list_get_nth(l, -2);
    NOIA_ASSERT(strcmp("3", e) == 0,
               "Last but one element should be '3' (is '%s')", e);

    e = noia_list_get_nth(l, -5);
    NOIA_ASSERT(strcmp("0", e) == 0,
               "First element should be '0' (is '%s')", e);

    e = noia_list_get_nth(l, 5);
    NOIA_ASSERT(e == NULL,
               "Over-last element should be '(nil)' (is '%s')", e);

    e = noia_list_get_nth(l, -6);
    NOIA_ASSERT(e == NULL,
               "Over-fist element should be '(nil)' (is '%s')", e);

    noia_list_free(l);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_from_begining(void)
{
    char* a[] = {"3", "4"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));
    noia_list_append(l, strdup("4"));

    noia_list_remove(l, "1", (NoiaCompareFunc) strcmp);
    noia_list_remove(l, "2", (NoiaCompareFunc) strcmp);

    NOIA_ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_from_end(void)
{
    char* a[] = {"1", "2"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));
    noia_list_append(l, strdup("4"));

    noia_list_remove(l, "4", (NoiaCompareFunc) strcmp);
    noia_list_remove(l, "3", (NoiaCompareFunc) strcmp);

    NOIA_ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_from_inside(void)
{
    char* a[] = {"1", "4"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));
    noia_list_append(l, strdup("4"));

    noia_list_remove(l, "3", (NoiaCompareFunc) strcmp);
    noia_list_remove(l, "2", (NoiaCompareFunc) strcmp);

    NOIA_ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_all_equal(void)
{
    char* a[] = {"1", "3"};

    NoiaList* l = noia_list_new(free);
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("1"));
    noia_list_append(l, strdup("2"));
    noia_list_append(l, strdup("3"));
    noia_list_append(l, strdup("2"));

    noia_list_remove_all(l, "2", (NoiaCompareFunc) strcmp);

    NOIA_ASSERT_LIST(l, a);
    noia_list_free(l);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_subtract_none(void)
{
    char* a[] = {"1", "4"};
    char* m[] = {"1", "4"};
    char* s[] = {"2", "3"};

    NoiaList* minuend = noia_list_new(free);
    noia_list_append(minuend, strdup("1"));
    noia_list_append(minuend, strdup("4"));

    NoiaList* subtrahent = noia_list_new(free);
    noia_list_append(subtrahent, strdup("2"));
    noia_list_append(subtrahent, strdup("3"));

    NoiaList* difference = noia_list_subtract(minuend, subtrahent,
                                              (NoiaCompareFunc) strcmp,
                                              (NoiaDuplicateFunc) strdup);

    NOIA_ASSERT_LIST(difference, a);
    NOIA_ASSERT_LIST(minuend, m);
    NOIA_ASSERT_LIST(subtrahent, s);

    noia_list_free(difference);
    noia_list_free(subtrahent);
    noia_list_free(minuend);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_subtract_some(void)
{
    char* d[] = {"1", "3"};
    char* m[] = {"1", "2", "3", "4"};
    char* s[] = {"2", "4", "5"};

    NoiaList* minuend = noia_list_new(free);
    noia_list_append(minuend, strdup("1"));
    noia_list_append(minuend, strdup("2"));
    noia_list_append(minuend, strdup("3"));
    noia_list_append(minuend, strdup("4"));

    NoiaList* subtrahent = noia_list_new(free);
    noia_list_append(subtrahent, strdup("2"));
    noia_list_append(subtrahent, strdup("4"));
    noia_list_append(subtrahent, strdup("5"));

    NoiaList* difference = noia_list_subtract(minuend, subtrahent,
                                              (NoiaCompareFunc) strcmp,
                                              (NoiaDuplicateFunc) strdup);

    NOIA_ASSERT_LIST(difference, d);
    NOIA_ASSERT_LIST(minuend, m);
    NOIA_ASSERT_LIST(subtrahent, s);

    noia_list_free(difference);
    noia_list_free(subtrahent);
    noia_list_free(minuend);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_subtract_all(void)
{
    char* m[] = {"1", "2", "3", "4"};
    char* s[] = {"1", "2", "3", "4", "5"};

    NoiaList* minuend = noia_list_new(free);
    noia_list_append(minuend, strdup("1"));
    noia_list_append(minuend, strdup("2"));
    noia_list_append(minuend, strdup("3"));
    noia_list_append(minuend, strdup("4"));

    NoiaList* subtrahent = noia_list_new(free);
    noia_list_append(subtrahent, strdup("1"));
    noia_list_append(subtrahent, strdup("2"));
    noia_list_append(subtrahent, strdup("3"));
    noia_list_append(subtrahent, strdup("4"));
    noia_list_append(subtrahent, strdup("5"));

    NoiaList* difference = noia_list_subtract(minuend, subtrahent,
                                              (NoiaCompareFunc) strcmp,
                                              (NoiaDuplicateFunc) strdup);

    int difference_length = noia_list_recalculate_length(difference);
    NOIA_ASSERT(difference_length == 0, "Difference should be empty "
                "(length is '%d')", difference_length);

    NOIA_ASSERT_LIST(minuend, m);
    NOIA_ASSERT_LIST(subtrahent, s);

    noia_list_free(difference);
    noia_list_free(subtrahent);
    noia_list_free(minuend);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should_append_values),
            NOIA_TEST(should_prepend_values),
            NOIA_TEST(should_get_nth),
            NOIA_TEST(should_remove_from_begining),
            NOIA_TEST(should_remove_from_end),
            NOIA_TEST(should_remove_from_inside),
            NOIA_TEST(should_remove_all_equal),
            NOIA_TEST(should_subtract_none),
            NOIA_TEST(should_subtract_some),
            NOIA_TEST(should_subtract_all),
        };

    return noia_test_run("List", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

