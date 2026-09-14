#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdio.h>

static int test_failures = 0;

static void check(const char *what, int condition) {
    printf("  [%s] %s\n", condition ? "PASS" : "FAIL", what);
    if (!condition) {
        test_failures++;
    }
}

static int test_report(const char *suite) {
    printf("\n%s: %s (%d failing checks)\n\n",
           suite,
           test_failures ? "FAILURES" : "all checks passed",
           test_failures);
    return test_failures != 0;
}

#endif // TEST_HELPERS_H
