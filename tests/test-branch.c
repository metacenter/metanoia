// file: tests-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-branch.h"
#include "tests-suit.h"

#include <malloc.h>

//------------------------------------------------------------------------------

NoiaTestResult should_append_and_prepend_values()
{
    char* a[] = {"1", "2", "3"};
    char* a1[] = {"11", "12"};
    char* a2[] = {"21", "22"};

    NoiaBranch* b = noia_branch_new();
    NoiaBranch* b1 = noia_branch_new();
    NoiaBranch* b2 = noia_branch_new();
    NoiaBranch* b3 = noia_branch_new();
    NoiaBranch* b11 = noia_branch_new();
    NoiaBranch* b12 = noia_branch_new();
    NoiaBranch* b21 = noia_branch_new();
    NoiaBranch* b22 = noia_branch_new();

    noia_branch_set_data(b,   strdup("0"));
    noia_branch_set_data(b1,  strdup("1"));
    noia_branch_set_data(b2,  strdup("2"));
    noia_branch_set_data(b3,  strdup("3"));
    noia_branch_set_data(b11, strdup("11"));
    noia_branch_set_data(b12, strdup("12"));
    noia_branch_set_data(b21, strdup("21"));
    noia_branch_set_data(b22, strdup("22"));

    noia_branch_append(b1, b11);
    noia_branch_append(b,  b1);
    noia_branch_append(b1, b12);
    noia_branch_append(b2, b21);
    noia_branch_append(b,  b2);
    noia_branch_append(b,  b3);
    noia_branch_append(b2, b22);

    ASSERT_CHAIN(b ->twigs, a);
    ASSERT_CHAIN(b1->twigs, a1);
    ASSERT_CHAIN(b2->twigs, a2);

    noia_branch_free(b, free);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    NoiaTest test[] = {
            NOIA_TEST(should_append_and_prepend_values),
        };

    return noia_test_run("List", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

