#include "./test_suite_001.h"
#include <limits.h>
#include <stdio.h>


int main(int argc, char * argv[]) {
    (void) argc;
    (void) argv;

    size_t num_tests = 0;

    for (size_t i = 0; test_suite_001_get_test(i) != NULL; i++) {
        test_suite_001_get_test(i)();
        num_tests++;
    }

    printf("Succesfully runned %zu tests.\n", num_tests);

    return 0;
};
