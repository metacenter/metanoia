// file: tests-list.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-branch.h"
#include "tests-suit.h"

#include <malloc.h>

#define NOIA_MAKE_BRANCHES() \
    NoiaBranch* b = noia_branch_new();\
    NoiaBranch* b1 = noia_branch_new();\
    NoiaBranch* b2 = noia_branch_new();\
    NoiaBranch* b3 = noia_branch_new();\
    NoiaBranch* b11 = noia_branch_new();\
    NoiaBranch* b12 = noia_branch_new();\
    NoiaBranch* b13 = noia_branch_new();\
    NoiaBranch* b21 = noia_branch_new();\
    NoiaBranch* b22 = noia_branch_new();\
    NoiaBranch* b23 = noia_branch_new();\
    NoiaBranch* b31 = noia_branch_new();\
    NoiaBranch* b32 = noia_branch_new();\
    NoiaBranch* b33 = noia_branch_new();\
    noia_branch_set_data(b,   strdup("0"));\
    noia_branch_set_data(b1,  strdup("1"));\
    noia_branch_set_data(b2,  strdup("2"));\
    noia_branch_set_data(b3,  strdup("3"));\
    noia_branch_set_data(b11, strdup("11"));\
    noia_branch_set_data(b12, strdup("12"));\
    noia_branch_set_data(b13, strdup("13"));\
    noia_branch_set_data(b21, strdup("21"));\
    noia_branch_set_data(b22, strdup("22"));\
    noia_branch_set_data(b23, strdup("23"));\
    noia_branch_set_data(b31, strdup("31"));\
    noia_branch_set_data(b32, strdup("32"));\
    noia_branch_set_data(b33, strdup("33"))

#define NOIA_GLUE_BRANCHES() \
    noia_branch_append(b3, b31);\
    noia_branch_append(b1, b11);\
    noia_branch_append(b,  b1);\
    noia_branch_append(b1, b12);\
    noia_branch_append(b2, b21);\
    noia_branch_append(b,  b2);\
    noia_branch_append(b3, b32);\
    noia_branch_append(b2, b22);\
    noia_branch_append(b3, b33);\
    noia_branch_append(b,  b3);\
    noia_branch_append(b2, b23);\
    noia_branch_append(b1, b13)

//------------------------------------------------------------------------------

NoiaTestResult should_append_values()
{
    char* a[] = {"1", "2", "3"};
    char* a1[] = {"11", "12", "13"};
    char* a2[] = {"21", "22", "23"};
    char* a3[] = {"31", "32", "33"};

    NOIA_MAKE_BRANCHES();
    NOIA_GLUE_BRANCHES();

    NOIA_ASSERT_BRANCH(b,  NULL, a);
    NOIA_ASSERT_BRANCH(b1, b,    a1);
    NOIA_ASSERT_BRANCH(b2, b,    a2);
    NOIA_ASSERT_BRANCH(b3, b,    a3);

    noia_branch_free(b, free);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_prepend_values()
{
    char* a[] = {"1", "2", "3"};
    char* a1[] = {"11", "12", "13"};
    char* a2[] = {"21", "22", "23"};
    char* a3[] = {"31", "32", "33"};

    NOIA_MAKE_BRANCHES();

    noia_branch_prepend(b,  b3);
    noia_branch_prepend(b2, b23);
    noia_branch_prepend(b1, b13);
    noia_branch_prepend(b1, b12);
    noia_branch_prepend(b3, b33);
    noia_branch_prepend(b,  b2);
    noia_branch_prepend(b1, b11);
    noia_branch_prepend(b3, b32);
    noia_branch_prepend(b2, b22);
    noia_branch_prepend(b,  b1);
    noia_branch_prepend(b2, b21);
    noia_branch_prepend(b3, b31);

    NOIA_ASSERT_BRANCH(b,  NULL, a);
    NOIA_ASSERT_BRANCH(b1, b,    a1);
    NOIA_ASSERT_BRANCH(b2, b,    a2);
    NOIA_ASSERT_BRANCH(b3, b,    a3);

    noia_branch_free(b, free);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_insert_values()
{
    char* a[] = {"1", "2", "3", "4", "5"};

    NoiaBranch* b  = noia_branch_new();
    NoiaBranch* b1 = noia_branch_new();
    NoiaBranch* b2 = noia_branch_new();
    NoiaBranch* b3 = noia_branch_new();
    NoiaBranch* b4 = noia_branch_new();
    NoiaBranch* b5 = noia_branch_new();
    noia_branch_set_data(b,  strdup("0"));
    noia_branch_set_data(b1, strdup("1"));
    noia_branch_set_data(b2, strdup("2"));
    noia_branch_set_data(b3, strdup("3"));
    noia_branch_set_data(b4, strdup("4"));
    noia_branch_set_data(b5, strdup("5"));

    noia_branch_append(b, b2);
    noia_branch_insert_after(b2, b4);
    noia_branch_insert_before(b4, b3);
    noia_branch_insert_after(b4, b5);
    noia_branch_insert_before(b2, b1);

    NOIA_ASSERT_BRANCH(b, NULL, a);

    noia_branch_free(b, free);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_remove_branch()
{
    char* a[] = {"1", "3"};
    char* a1[] = {"11", "12", "13"};
    char* a2[] = {"21", "22", "23"};
    char* a3[] = {"31", "32", "33"};

    NOIA_MAKE_BRANCHES();
    NOIA_GLUE_BRANCHES();

    noia_branch_remove(b, b2);

    NOIA_ASSERT_BRANCH(b2, NULL, a2);

    NOIA_ASSERT_BRANCH(b,  NULL, a);
    NOIA_ASSERT_BRANCH(b1, b,    a1);
    NOIA_ASSERT_BRANCH(b3, b,    a3);

    noia_branch_free(b2, free);
    noia_branch_free(b, free);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_find_branch()
{
    char* a2[] = {"21", "22", "23"};

    NOIA_MAKE_BRANCHES();
    NOIA_GLUE_BRANCHES();

    NoiaBranch* f = noia_branch_find(b, "2", (NoiaBranchCompare) strcmp);

    NOIA_ASSERT_BRANCH(f, b, a2);

    noia_branch_free(b, free);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS();

    NoiaTest test[] = {
            NOIA_TEST(should_append_values),
            NOIA_TEST(should_prepend_values),
            NOIA_TEST(should_insert_values),
            NOIA_TEST(should_remove_branch),
            NOIA_TEST(should_find_branch),
        };

    return noia_test_run("Branch", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

