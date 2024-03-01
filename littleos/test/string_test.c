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

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_strlen),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
