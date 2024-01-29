#include "../exceptions.h"
#include "../shared/string.h"
#include "../base.h"
#include <string.h>


#define EXCEPTION_MSG_OK "No error."


typedef struct TRCLException {
    TRCLExceptionCode code;
    char const * message;
} TRCLException;


TRCLException * trcl_exception_alloc(
    TRCLExceptionCode code,
    char const * message
) {
    TRCLException * e = TRCL_ALLOC(TRCLException, 1);
    TRCL_ASSERT_ALLOC(e);

    e->code = code;
    e->message = str_copy_alloc(message);

    return e;
};


TRCLException * trcl_exception_ok_alloc(void) {
    TRCLException * e = trcl_exception_alloc(
        TRCL_EXCEPTION__OK,
        NULL
    );
    TRCL_ASSERT_ALLOC(e);

    trcl_exception_set_ok(e);

    return e;
};


void trcl_exception_destroy(TRCLException *e) {
    RETURN_IF_NULL(e);

    e->code = TRCL_EXCEPTION__OK;

    str_destroy(e->message);
    e->message = NULL;

    TRCL_FREE(e);
};


inline TRCLExceptionCode trcl_exception_get_code(
    TRCLException const *e
) {
    return e->code;
};


inline char const * trcl_exception_get_message(
    TRCLException const *e
) {
    return e->message;
};



inline void trcl_exception_set_message(
    TRCLException *e,
    char const *message
) {
    e->message = str_set_alloc((char *) e->message, message);
};


inline void trcl_exception_set_ok(TRCLException *e) {
    e->code = TRCL_EXCEPTION__OK;
    trcl_exception_set_message(e, EXCEPTION_MSG_OK);
};
