#include "string.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

static void test_strlen(void **state) {
    (void)state; /* unused */
    assert_true(5 == strlen("hello"));
    assert_true(0 == strlen(""));
    // char *str = NULL;
    // assert_true(0 == strlen(str));
}

static void test_strnlen(void **state) {
    (void)state; /* unused */
    assert_true(5 == strnlen("hello", 6));
    assert_true(5 == strnlen("hello", 5));
    assert_true(4 == strnlen("hello", 4));
    assert_true(0 == strnlen("hello", 0));
    assert_true(0 == strnlen("", 5));
    assert_true(0 == strnlen("", 0));
    // assert_true(0 == strnlen("", -1));
}

static void test_strcmp(void **state) {
    (void)state; /* unused */
    assert_true(strcmp("abc", "abcd") < 0);
    assert_true(strcmp("abc", "abc") == 0);
    assert_true(strcmp("abc", "ab") > 0);
    assert_true(strcmp("abc", "Abc") > 0);
    assert_true(strcmp("abc", "") > 0);
    assert_true(strcmp("", "abc") < 0);
    assert_true(strcmp("", "") == 0);

    assert_true(strcmp("aBc", "ab") < 0);
}

static void test_strncmp(void **state) {
    (void)state; /* unused */
    assert_true(strncmp("abc", "abc", 5) == 0);
    assert_true(strncmp("abc", "abc", 3) == 0);
    assert_true(strncmp("abc", "abc", 2) == 0);
    assert_true(strncmp("abc", "abc", 0) == 0);
    assert_true(strncmp("abc", "Abc", 0) == 0);

    assert_true(strncmp("", "", 0) == 0);
    assert_true(strncmp("", "", 5) == 0);

    assert_true(strncmp("", "abc", 3) < 0);
    assert_true(strncmp("", "abc", 0) == 0);
    assert_true(strncmp("aBc", "abc", 3) < 0);

    assert_true(strncmp("abc", "aBc", 3) > 0);
    assert_true(strncmp("abc", "", 2) > 0);
    assert_true(strncmp("abc", "", 0) == 0);
}

static void test_strncasecmp(void **state) {
    (void)state; /* unused */
    assert_true(strncasecmp("abc", "abc", 5) == 0);
    assert_true(strncasecmp("abc", "abc", 3) == 0);
    assert_true(strncasecmp("abc", "abc", 2) == 0);
    assert_true(strncasecmp("abc", "abc", 0) == 0);
    assert_true(strncasecmp("abc", "Abc", 0) == 0);

    assert_true(strncasecmp("", "", 0) == 0);
    assert_true(strncasecmp("", "", 5) == 0);

    assert_true(strncasecmp("", "abc", 3) < 0);
    assert_true(strncasecmp("", "abc", 0) == 0);
    assert_true(strncasecmp("aBc", "abc", 3) == 0);

    assert_true(strncasecmp("abc", "aBc", 3) == 0);
    assert_true(strncasecmp("abc", "", 2) > 0);
    assert_true(strncasecmp("abc", "", 0) == 0);

    assert_true(strncasecmp("a", "[", 3) > 0);
    assert_true(strncasecmp("A", "[", 3) < 0);
}

static void test_memcmp(void **state) {
    (void)state; /* unused */
    assert_true(memcmp((uint8_t[]){1, 2, 3}, (uint8_t[]){1, 2, 3}, 3) == 0);
    assert_true(memcmp((uint8_t[]){1, 2, 3}, (uint8_t[]){1, 2, 3}, 2) == 0);
    assert_true(memcmp((uint8_t[]){1, 2, 3}, (uint8_t[]){1, 2, 3}, 0) == 0);
    assert_true(memcmp((uint8_t[]){1, 0, 3}, (uint8_t[]){1, 0, 3}, 3) == 0);
    assert_true(memcmp((uint8_t[]){1, 0, 3}, (uint8_t[]){1, 0, 3}, 2) == 0);
    assert_true(memcmp((uint8_t[]){1, 0, 4}, (uint8_t[]){1, 0, 3}, 3) > 0);
    assert_true(memcmp((uint8_t[]){1, 0, 4}, (uint8_t[]){1, 0, 3}, 2) == 0);
    assert_true(memcmp((uint8_t[]){1, 0, 4}, (uint8_t[]){1, 0, 3}, 0) == 0);
    assert_true(memcmp((uint8_t[]){1, 0, 3}, (uint8_t[]){1, 0, 4}, 3) < 0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_strlen),      cmocka_unit_test(test_strnlen),
        cmocka_unit_test(test_strcmp),      cmocka_unit_test(test_strncmp),
        cmocka_unit_test(test_strncasecmp), cmocka_unit_test(test_memcmp),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
