// file: tests-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-list.h"
#include "tests-suit.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraTestResult should_append_values()
{
    char* a[] = {"1", "2", "3"};

    AuraList* l = aura_list_new(free);
    aura_list_append(l, strdup("1"));
    aura_list_append(l, strdup("2"));
    aura_list_append(l, strdup("3"));

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_prepend_values()
{
    char* a[] = {"3", "2", "1"};

    AuraList* l = aura_list_new(free);
    aura_list_prepend(l, strdup("1"));
    aura_list_prepend(l, strdup("2"));
    aura_list_prepend(l, strdup("3"));

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_begining()
{
    char* a[] = {"2", "3"};

    AuraList* l = aura_list_new(free);
    aura_list_append(l, strdup("1"));
    aura_list_append(l, strdup("2"));
    aura_list_append(l, strdup("3"));

    aura_list_remove(l, "1", (AuraCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_end()
{
    char* a[] = {"1", "2"};

    AuraList* l = aura_list_new(free);
    aura_list_append(l, strdup("1"));
    aura_list_append(l, strdup("2"));
    aura_list_append(l, strdup("3"));

    aura_list_remove(l, "3", (AuraCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_inside()
{
    char* a[] = {"1", "3"};

    AuraList* l = aura_list_new(free);
    aura_list_append(l, strdup("1"));
    aura_list_append(l, strdup("2"));
    aura_list_append(l, strdup("3"));

    aura_list_remove(l, "2", (AuraCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_all_equal()
{
    char* a[] = {"1", "3"};

    AuraList* l = aura_list_new(free);
    aura_list_append(l, strdup("2"));
    aura_list_append(l, strdup("1"));
    aura_list_append(l, strdup("2"));
    aura_list_append(l, strdup("3"));
    aura_list_append(l, strdup("2"));

    aura_list_remove_all(l, "2", (AuraCompareFunc) strcmp);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    AuraTest test[] = {
            AURA_TEST(should_append_values),
            AURA_TEST(should_prepend_values),
            AURA_TEST(should_remove_from_begining),
            AURA_TEST(should_remove_from_end),
            AURA_TEST(should_remove_from_inside),
            AURA_TEST(should_remove_all_equal),
        };

    return aura_test_run("List", test, AURA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

