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

#define AURA_TEST(test) {#test,test}
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
            printf("FAILURE\n\n");
            return 1;
        }
        printf("\n");
    }
    printf("SUCCESS\n\n");
    return 0;
}

#endif // __AURA_TESTS_SUIT_H__

