#ifndef TRCL_BASE_H
#define TRCL_BASE_H


#include "./config.h"


#if defined(__cplusplus)
    #define CLITERAL(type) type
#else
    #define CLITERAL(type) (type)
#endif


#ifndef TRCL_ALLOC
#include <stdlib.h>
#define TRCL_ALLOC(type, n) ((type *) malloc(sizeof(type) * ((size_t) n)))
#endif


#ifndef TRCL_REALLOC
#include <stdlib.h>
#define TRCL_REALLOC(ptr, type, n) realloc(ptr, sizeof(type) * ((size_t)n))
#endif


#ifndef TRCL_FREE
#include <stdlib.h>
#define TRCL_FREE(ptr) if (!IS_NULL(ptr)) {\
    free((void *) (ptr));\
    ptr = NULL;\
}
#endif


#ifndef IS_NULL
#define IS_NULL(ptr) ((ptr) == NULL)
#endif


#ifndef TRCL_ASSERT_ALLOC
#include <assert.h>
#define TRCL_ASSERT_ALLOC(ptr) assert(!IS_NULL(ptr))
#endif


#ifndef TRCL_ASSERT_ALLOC_OR
#define TRCL_ASSERT_ALLOC_OR(ptr, alternative) if(IS_NULL(ptr)) {\
    alternative \
}
#endif


#ifndef RETURN_IF_NULL
#define RETURN_IF_NULL(ptr) if (IS_NULL(ptr)) {\
  return;\
}
#endif


#ifndef RETURN_V_IF_NULL
#define RETURN_V_IF_NULL(ptr, value) if (IS_NULL(ptr)) {\
  return value;\
}
#endif


#ifndef LOG
#include <stdio.h>
#define LOG printf
#endif


#ifndef FALSE
#define FALSE 0
#endif


#ifndef TRUE
#define TRUE 1
#endif

#endif
