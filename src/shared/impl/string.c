#include "../string.h"
#include "../../base.h"
#include <string.h>
#include <stdint.h>


char * str_alloc(size_t length) {
    char * dst = TRCL_ALLOC(char, length + 1);
    TRCL_ASSERT_ALLOC(dst);

    memset(dst, 0, length + 1);

    return dst;
};


char * str_copy_alloc(char const *src) {
    RETURN_V_IF_NULL(src, NULL);

    size_t length = strlen(src);

    char * dst = TRCL_ALLOC(char, length + 1);
    TRCL_ASSERT_ALLOC(dst);

    strncpy(dst, src, length + 1);

    return dst;
};


char * str_set_alloc(char *dst, char const *src) {
    if (IS_NULL(src)) {
        str_destroy(dst);
        return NULL;
    }

    size_t length = strlen(src);

    dst = TRCL_REALLOC(dst, char, length + 1);
    TRCL_ASSERT_ALLOC(dst);

    strncpy(dst, src, length + 1);

    return dst;
};


inline void str_destroy(char const *str) {
    RETURN_IF_NULL(str);
    TRCL_FREE(str);
};


char * str_concat(char *dst, char const *src) {
    dst = strcat(dst, src);
    return dst;
};
