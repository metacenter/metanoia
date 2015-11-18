// file: tests-suit.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_TESTS_SUIT_H__
#define __NOIA_TESTS_SUIT_H__

/// @file
/// @todo Add NOIA prefixes.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

#define ASSERT_POOL_SIZE(POOL, ARRAY_SIZE, SIZE) { \
    NOIA_ASSERT(ARRAY_SIZE == noia_pool_get_size(POOL), \
                "Number of elements should be '%lu' (is '%u')", \
                ARRAY_SIZE, noia_pool_get_size(POOL)); \
    NOIA_ASSERT(SIZE == noia_pool_get_alocation_size(POOL), \
                "Number of allocated elements should be '%u' (is '%u')", \
                SIZE, noia_pool_get_alocation_size(POOL)); } \

#define ASSERT_POOL(POOL, ARRAY, TYPE, SIZE) { \
    ASSERT_POOL_SIZE(POOL, ARRAY_LEN(ARRAY), SIZE) \
    TYPE *ptr = noia_pool_top(POOL); \
    TYPE array_data = ARRAY[ARRAY_LEN(ARRAY)-1]; \
    NOIA_ASSERT(array_data == *ptr, \
                "Top element should be '%d' (is '%d')", \
                array_data, *ptr); \
    for (unsigned i = 0; i < ARRAY_LEN(ARRAY); ++i) { \
        TYPE* ptr = noia_pool_get(POOL, i); \
        NOIA_ASSERT(ARRAY[i] == *ptr, \
                    "Element should be '%d' (is '%d')", \
                    ARRAY[i], *ptr); }}

#define FILL_POOL(POOL, ARRAY, TYPE) { \
    for (unsigned i = 0; i < ARRAY_LEN(ARRAY); ++i) { \
        TYPE* ptr = noia_pool_add(POOL); \
        *ptr = ARRAY[i]; }};

#define NOIA_ASSERT_CHAIN_LEN(CHAIN, LEN) \
    NOIA_ASSERT(LEN == chain_recalculate_length(CHAIN), \
                "Calculated chain length should be %d (is %d)", \
                LEN, chain_recalculate_length(CHAIN));

#define ASSERT_CHAIN(CHAIN, ARRAY) { \
    int i = 0, len = ARRAY_LEN(ARRAY); \
    NOIA_ASSERT(len == chain_len(CHAIN), \
                "Stored chain length should be %d (is %d)", \
                len, chain_len(CHAIN)); \
    NOIA_ASSERT_CHAIN_LEN(CHAIN, len); \
    for (Link* link = CHAIN->first; link; link = link->next, ++i) { \
        char* chain_data = link->data; \
        char* array_data = ARRAY[i]; \
        NOIA_ASSERT(strcmp(chain_data, array_data) == 0, \
                    "Chain data should be '%s' (is '%s')", \
                    array_data, chain_data); }}

#define ASSERT_LIST(LIST, ARRAY) { \
    int i = 0, len = ARRAY_LEN(ARRAY); \
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

#define NOIA_ASSERT_TRUNK(BRANCH, TRUNK) \
    NOIA_ASSERT(BRANCH->trunk == TRUNK, \
                "Trunk should be '%p', (is '%p')", \
                (void*) BRANCH->trunk, (void*) TRUNK); \

#define ASSERT_BRANCH(BRANCH, TRUNK, ARRAY) \
    NOIA_ASSERT(BRANCH != NULL, "Branch should not be NULL"); \
    NOIA_ASSERT_TRUNK(BRANCH, TRUNK); \
    ASSERT_CHAIN(BRANCH->twigs, ARRAY);

#define NOIA_TEST(test) {#test,test}
#define ARRAY_LEN(a) (sizeof(a)/sizeof(*a))
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
    int result = 0;
    printf("Run test suit '%s'\n\n", suit_name);
    for(int n = 0; n < N; ++n) {
        NOIA_TEST_PRINTF("Run test %d - '%s'\n", n+1, test[n].name);
        NoiaTestResult check = test[n].check();
        if (check != NOIA_TEST_SUCCESS) {
            result = 1;
        }
        NOIA_TEST_PRINTF("\n");
    }
    if (result) {
        printf("FAILURE\n\n");
    } else {
        printf("SUCCESS\n\n");
    }
    return result;
}

#endif // __NOIA_TESTS_SUIT_H__

