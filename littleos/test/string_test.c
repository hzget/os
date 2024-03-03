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
    //    char *str = NULL;
    //   assert_true(0 == strlen(str));
}

static void test_strnlen(void **state) {
    (void)state; /* unused */
    assert_true(5 == strnlen("hello", 6));
    assert_true(5 == strnlen("hello", 5));
    assert_true(4 == strnlen("hello", 4));
    assert_true(0 == strnlen("hello", 0));
    assert_true(0 == strnlen("", 5));
    assert_true(0 == strnlen("", 0));
    //    assert_true(0 == strnlen("", -1));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_strlen),
        cmocka_unit_test(test_strnlen),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
