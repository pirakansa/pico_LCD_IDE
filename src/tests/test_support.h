#ifndef _PICO_LCD_SRC_TESTS_TEST_SUPPORT_
#define _PICO_LCD_SRC_TESTS_TEST_SUPPORT_

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const char *suite_name;
    const char *current_test;
    int total;
    int passed;
} test_suite_t;

static inline void test_suite_begin(test_suite_t *suite, const char *suite_name) {
    suite->suite_name = suite_name;
    suite->current_test = NULL;
    suite->total = 0;
    suite->passed = 0;
    printf("== %s ==\n", suite_name);
}

static inline void test_case_begin(test_suite_t *suite, const char *test_name) {
    suite->current_test = test_name;
    suite->total++;
}

static inline void test_case_pass(test_suite_t *suite) {
    suite->passed++;
    printf("  [PASS] %s\n", suite->current_test);
}

static inline void test_suite_end(test_suite_t *suite) {
    printf("== %s: %d/%d passed ==\n", suite->suite_name, suite->passed, suite->total);
}

static inline void test_fail_int_impl(
    test_suite_t *suite,
    const char *file,
    int line,
    const char *assertion_name,
    int expected,
    int actual
) {
    fprintf(
        stderr,
        "[FAIL] %s :: %s\n  location: %s:%d\n  assertion: %s\n  expected: %d\n  actual: %d\n",
        suite->suite_name,
        suite->current_test,
        file,
        line,
        assertion_name,
        expected,
        actual
    );
    exit(1);
}

static inline void test_fail_ptr_impl(
    test_suite_t *suite,
    const char *file,
    int line,
    const char *assertion_name,
    const void *expected,
    const void *actual
) {
    fprintf(
        stderr,
        "[FAIL] %s :: %s\n  location: %s:%d\n  assertion: %s\n  expected: %p\n  actual: %p\n",
        suite->suite_name,
        suite->current_test,
        file,
        line,
        assertion_name,
        expected,
        actual
    );
    exit(1);
}

#define ASSERT_INT(suite, name, expected, actual) \
    do { \
        int expected_value__ = (expected); \
        int actual_value__ = (actual); \
        if (expected_value__ != actual_value__) { \
            test_fail_int_impl((suite), __FILE__, __LINE__, (name), expected_value__, actual_value__); \
        } \
    } while (0)

#define ASSERT_PTR(suite, name, expected, actual) \
    do { \
        const void *expected_value__ = (const void *)(expected); \
        const void *actual_value__ = (const void *)(actual); \
        if (expected_value__ != actual_value__) { \
            test_fail_ptr_impl((suite), __FILE__, __LINE__, (name), expected_value__, actual_value__); \
        } \
    } while (0)

#define RUN_TEST(suite, fn) \
    do { \
        test_case_begin((suite), #fn); \
        fn((suite)); \
        test_case_pass((suite)); \
    } while (0)

#endif
