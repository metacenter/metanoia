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

NoiaTestResult should_subtract_none()
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

    ASSERT_LIST(difference, a);
    ASSERT_LIST(minuend, m);
    ASSERT_LIST(subtrahent, s);

    noia_list_free(difference);
    noia_list_free(subtrahent);
    noia_list_free(minuend);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_subtract_some()
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

    ASSERT_LIST(difference, d);
    ASSERT_LIST(minuend, m);
    ASSERT_LIST(subtrahent, s);

    noia_list_free(difference);
    noia_list_free(subtrahent);
    noia_list_free(minuend);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_subtract_all()
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

    ASSERT_LIST(minuend, m);
    ASSERT_LIST(subtrahent, s);

    noia_list_free(difference);
    noia_list_free(subtrahent);
    noia_list_free(minuend);

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
            NOIA_TEST(should_subtract_none),
            NOIA_TEST(should_subtract_some),
            NOIA_TEST(should_subtract_all),
        };

    return noia_test_run("List", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

