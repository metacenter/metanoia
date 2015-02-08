// file: tests-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-list.h"
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

#define ASSERT_LIST(LIST, ARRAY) \
    int i = 0, len = ARRAY_LEN(ARRAY, int); \
    AURA_ASSERT(len == aura_list_len(LIST), \
                "Stored list length should be %d (is %d)", \
                len, aura_list_len(LIST)); \
    AURA_ASSERT(len == aura_list_len(LIST), \
                "Calculated list length should be %d (is %d)", \
                len, aura_list_recalculate_length(LIST)); \
    FOR_EACH(LIST, link) { \
        intptr_t list_data = (intptr_t) link->data; \
        int array_data = ARRAY[i++]; \
        AURA_ASSERT(list_data == array_data, \
                    "List data should be %d (is %" PRIdPTR ")", \
                    array_data, list_data); }

//------------------------------------------------------------------------------

AuraTestResult should_append_values()
{
    int a[] = {1, 2, 3};

    AuraList* l = aura_list_new(NULL);
    aura_list_append(l, (void*) 1);
    aura_list_append(l, (void*) 2);
    aura_list_append(l, (void*) 3);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_prepend_values()
{
    int a[] = {3, 2, 1};

    AuraList* l = aura_list_new(NULL);
    aura_list_prepend(l, (void*) 1);
    aura_list_prepend(l, (void*) 2);
    aura_list_prepend(l, (void*) 3);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_begining()
{
    int a[] = {2, 3};

    AuraList* l = aura_list_new(NULL);
    aura_list_append(l, (void*) 1);
    aura_list_append(l, (void*) 2);
    aura_list_append(l, (void*) 3);

    aura_list_remove(l, (void*) 1, (AuraCompareFunc) compare);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_end()
{
    int a[] = {1, 2};

    AuraList* l = aura_list_new(NULL);
    aura_list_append(l, (void*) 1);
    aura_list_append(l, (void*) 2);
    aura_list_append(l, (void*) 3);

    aura_list_remove(l, (void*) 3, (AuraCompareFunc) compare);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_from_inside()
{
    int a[] = {1, 3};

    AuraList* l = aura_list_new(NULL);
    aura_list_append(l, (void*) 1);
    aura_list_append(l, (void*) 2);
    aura_list_append(l, (void*) 3);

    aura_list_remove(l, (void*) 2, (AuraCompareFunc) compare);

    ASSERT_LIST(l, a);
    aura_list_free(l);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

AuraTestResult should_remove_all_equal()
{
    int a[] = {1, 3};

    AuraList* l = aura_list_new(NULL);
    aura_list_append(l, (void*) 2);
    aura_list_append(l, (void*) 1);
    aura_list_append(l, (void*) 2);
    aura_list_append(l, (void*) 3);
    aura_list_append(l, (void*) 2);

    aura_list_remove_all(l, (void*) 2, (AuraCompareFunc) compare);

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

