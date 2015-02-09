// file: tests-suit.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_TESTS_SUIT_H__
#define __AURA_TESTS_SUIT_H__

#include <stdio.h>
#include <string.h>

#define AURA_ASSERT(expr, ...) \
    if (!(expr)) { \
        printf("[FAIL] "); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        return AURA_TEST_ERROR; \
    } else { \
        printf("[ OK ] "); \
        printf(__VA_ARGS__); \
        printf("\n"); \
    }

#define ASSERT_CHAIN(CHAIN, ARRAY) { \
    int i = 0, len = ARRAY_LEN(ARRAY, char*); \
    AURA_ASSERT(len == chain_len(CHAIN), \
                "Stored chain length should be %d (is %d)", \
                len, chain_len(CHAIN)); \
    AURA_ASSERT(len == chain_len(CHAIN), \
                "Calculated chain length should be %d (is %d)", \
                len, chain_recalculate_length(CHAIN)); \
    for (Link* link = CHAIN->first; link; link = link->next, ++i) { \
        char* chain_data = link->data; \
        char* array_data = ARRAY[i]; \
        AURA_ASSERT(strcmp(chain_data, array_data) == 0, \
                    "Chain data should be '%s' (is '%s')", \
                    array_data, chain_data); }}

#define ASSERT_LIST(LIST, ARRAY) { \
    int i = 0, len = ARRAY_LEN(ARRAY, char*); \
    AURA_ASSERT(len == aura_list_len(LIST), \
                "Stored list length should be %d (is %d)", \
                len, aura_list_len(LIST)); \
    AURA_ASSERT(len == aura_list_len(LIST), \
                "Calculated list length should be %d (is %d)", \
                len, aura_list_recalculate_length(LIST)); \
    FOR_EACH(LIST, link) { \
        char* list_data = link->data; \
        char* array_data = ARRAY[i++]; \
        AURA_ASSERT(strcmp(list_data, array_data) == 0, \
                    "List data should be '%s' (is '%s')", \
                    array_data, list_data); }}

#define AURA_TEST(test) {#test,test}
#define ARRAY_LEN(a,t) (sizeof(a)/sizeof(t))
#define AURA_NUM_TESTS(array) (sizeof(array)/(sizeof(AuraTest)))


typedef enum {
    AURA_TEST_ERROR = 0,
    AURA_TEST_SUCCESS = 1,
} AuraTestResult;

typedef AuraTestResult (*AuraTestFunc) ();

typedef struct {
    char* name;
    AuraTestFunc check;
} AuraTest;

int aura_test_run(const char* suit_name, AuraTest* test, int N)
{
    printf("Run test suit '%s'\n\n", suit_name);
    for(int n = 0; n < N; ++n) {
        printf("Run test %d - '%s'\n", n+1, test[n].name);
        AuraTestResult result = test[n].check();
        if (result != AURA_TEST_SUCCESS) {
            printf("\nFAILURE\n\n");
            return 1;
        }
        printf("\n");
    }
    printf("SUCCESS\n\n");
    return 0;
}

#endif // __AURA_TESTS_SUIT_H__

