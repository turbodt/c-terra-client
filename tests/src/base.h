#ifndef TRCL_TESTS_BASE_H
#define TRCL_TESTS_BASE_H


#include "../../src/base.h"


#define TEST_NAME(test_name) test_##test_name
#define TEST_H(test_name) static void TEST_NAME(test_name)(void);
#define TEST(test_name, test_code)                \
    void TEST_NAME(test_name)(void) {      \
        printf("Running test: %s... ", #test_name);\
        fflush(stdout);                           \
        test_code;                                \
        printf("Done\n");                         \
    }

#ifndef LOG
#include <stdio.h>
#define LOG printf
#endif

#include <assert.h>
#define ASSERT assert
#define ASSERT_FALSE(a) ASSERT(!(a))
#define ASSERT_TRUE(a) ASSERT(a)
#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_LEQ(a, b) ASSERT((a) <= (b))
#define ASSERT_LT(a, b) ASSERT((a) < (b))
#define ASSERT_GEQ(a, b) ASSERT((a) >= (b))
#define ASSERT_GT(a, b) ASSERT((a) > (b))
#define ASSERT_NULL(a) ASSERT(IS_NULL(a))
#define ASSERT_NOT_NULL(a) ASSERT(!IS_NULL(a))

#endif

