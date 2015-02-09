// file: tests-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "../src/utils-branch.h"
#include "tests-suit.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraTestResult should_append_and_prepend_values()
{
    char* a[] = {"1", "2", "3"};
    char* a1[] = {"11", "12"};
    char* a2[] = {"21", "22"};

    AuraBranch* b = aura_branch_new();
    AuraBranch* b1 = aura_branch_new();
    AuraBranch* b2 = aura_branch_new();
    AuraBranch* b3 = aura_branch_new();
    AuraBranch* b11 = aura_branch_new();
    AuraBranch* b12 = aura_branch_new();
    AuraBranch* b21 = aura_branch_new();
    AuraBranch* b22 = aura_branch_new();

    aura_branch_set_data(b,   strdup("0"));
    aura_branch_set_data(b1,  strdup("1"));
    aura_branch_set_data(b2,  strdup("2"));
    aura_branch_set_data(b3,  strdup("3"));
    aura_branch_set_data(b11, strdup("11"));
    aura_branch_set_data(b12, strdup("12"));
    aura_branch_set_data(b21, strdup("21"));
    aura_branch_set_data(b22, strdup("22"));

    aura_branch_append(b1, b11);
    aura_branch_append(b,  b1);
    aura_branch_append(b1, b12);
    aura_branch_append(b2, b21);
    aura_branch_append(b,  b2);
    aura_branch_append(b,  b3);
    aura_branch_append(b2, b22);

    ASSERT_CHAIN(b ->twigs, a);
    ASSERT_CHAIN(b1->twigs, a1);
    ASSERT_CHAIN(b2->twigs, a2);

    aura_branch_free(b, free);
    return AURA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main()
{
    AuraTest test[] = {
            AURA_TEST(should_append_and_prepend_values),
        };

    return aura_test_run("List", test, AURA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

