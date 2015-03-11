// file: tests-suit.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_TESTS_SUIT_H__
#define __NOIA_TESTS_SUIT_H__

#include <stdio.h>
#include <string.h>

#define NOIA_ASSERT(expr, ...) \
    if (!(expr)) { \
        printf("[FAIL] "); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        return NOIA_TEST_ERROR; \
    } else { \
        printf("[ OK ] "); \
        printf(__VA_ARGS__); \
        printf("\n"); \
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

#define NOIA_TEST(test) {#test,test}
#define ARRAY_LEN(a,t) (sizeof(a)/sizeof(t))
#define NOIA_NUM_TESTS(array) (sizeof(array)/(sizeof(NoiaTest)))


typedef enum {
    NOIA_TEST_ERROR = 0,
    NOIA_TEST_SUCCESS = 1,
} NoiaTestResult;

typedef NoiaTestResult (*NoiaTestFunc) ();

typedef struct {
    char* name;
    NoiaTestFunc check;
} NoiaTest;

int noia_test_run(const char* suit_name, NoiaTest* test, int N)
{
    printf("Run test suit '%s'\n\n", suit_name);
    for(int n = 0; n < N; ++n) {
        printf("Run test %d - '%s'\n", n+1, test[n].name);
        NoiaTestResult result = test[n].check();
        if (result != NOIA_TEST_SUCCESS) {
            printf("\nFAILURE\n\n");
            return 1;
        }
        printf("\n");
    }
    printf("SUCCESS\n\n");
    return 0;
}

#endif // __NOIA_TESTS_SUIT_H__

