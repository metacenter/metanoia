// file: tests-suit.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_TESTS_SUIT_H__
#define __NOIA_TESTS_SUIT_H__

#include <stdio.h>
#include <string.h>

#define NOIA_INIT_TESTS() noia_test_init(argc, argv)

#define NOIA_TEST_PRINTF(...) if (!sQuiet) printf(__VA_ARGS__);

#define NOIA_ASSERT(expr, ...) \
    if (!(expr)) { \
        NOIA_TEST_PRINTF("[FAIL] "); \
        NOIA_TEST_PRINTF(__VA_ARGS__); \
        NOIA_TEST_PRINTF("\n"); \
        return NOIA_TEST_ERROR; \
    } else { \
        NOIA_TEST_PRINTF("[ OK ] "); \
        NOIA_TEST_PRINTF(__VA_ARGS__); \
        NOIA_TEST_PRINTF("\n"); \
    }

#define ASSERT_CHAIN(CHAIN, ARRAY) { \
    int i = 0, len = ARRAY_LEN(ARRAY, char*); \
    NOIA_ASSERT(len == chain_len(CHAIN), \
                "Stored chain length should be %d (is %d)", \
                len, chain_len(CHAIN)); \
    NOIA_ASSERT(len == chain_len(CHAIN), \
                "Calculated chain length should be %d (is %d)", \
                len, chain_recalculate_length(CHAIN)); \
    for (Link* link = CHAIN->first; link; link = link->next, ++i) { \
        char* chain_data = link->data; \
        char* array_data = ARRAY[i]; \
        NOIA_ASSERT(strcmp(chain_data, array_data) == 0, \
                    "Chain data should be '%s' (is '%s')", \
                    array_data, chain_data); }}

#define ASSERT_LIST(LIST, ARRAY) { \
    int i = 0, len = ARRAY_LEN(ARRAY, char*); \
    NOIA_ASSERT(len == noia_list_len(LIST), \
                "Stored list length should be %d (is %d)", \
                len, noia_list_len(LIST)); \
    NOIA_ASSERT(len == noia_list_len(LIST), \
                "Calculated list length should be %d (is %d)", \
                len, noia_list_recalculate_length(LIST)); \
    FOR_EACH(LIST, link) { \
        char* list_data = link->data; \
        char* array_data = ARRAY[i++]; \
        NOIA_ASSERT(strcmp(list_data, array_data) == 0, \
                    "List data should be '%s' (is '%s')", \
                    array_data, list_data); }}

#define ASSERT_BRANCH(BRANCH, TRUNK, ARRAY) \
    NOIA_ASSERT(BRANCH != NULL, "Branch should not be NULL"); \
    NOIA_ASSERT(BRANCH->trunk == TRUNK, \
                "Trunk should be '%p', (is '%p')", \
                (void*) BRANCH->trunk, (void*) TRUNK); \
    ASSERT_CHAIN(BRANCH->twigs, ARRAY);

#define NOIA_TEST(test) {#test,test}
#define ARRAY_LEN(a,t) (sizeof(a)/sizeof(t))
#define NOIA_NUM_TESTS(array) (sizeof(array)/(sizeof(NoiaTest)))

static bool sQuiet = false;

typedef enum {
    NOIA_TEST_ERROR = 0,
    NOIA_TEST_SUCCESS = 1,
} NoiaTestResult;

typedef NoiaTestResult (*NoiaTestFunc) ();

typedef struct {
    char* name;
    NoiaTestFunc check;
} NoiaTest;

void noia_test_init(int argc, char** argv)
{
    sQuiet = (argc > 1) && (strcmp(argv[1], "-q") == 0);
}

int noia_test_run(const char* suit_name, NoiaTest* test, int N)
{
    printf("Run test suit '%s'\n\n", suit_name);
    for(int n = 0; n < N; ++n) {
        NOIA_TEST_PRINTF("Run test %d - '%s'\n", n+1, test[n].name);
        NoiaTestResult result = test[n].check();
        if (result != NOIA_TEST_SUCCESS) {
            printf("\nFAILURE\n\n");
            return 1;
        }
        NOIA_TEST_PRINTF("\n");
    }
    printf("SUCCESS\n\n");
    return 0;
}

#endif // __NOIA_TESTS_SUIT_H__

