#ifndef TRCL_EXCEPTIONS_H
#define TRCL_EXCEPTIONS_H


typedef enum TRCLExceptionCode {
    TRCL_EXCEPTION__OK = 0x00,
    TRCL_EXCEPTION__UNKNOWN = 0x01,
    TRCL_EXCEPTION__INVALID_STATE = 0x02,
    TRCL_EXCEPTION__CANNOT_REQUEST = 0x04,
    TRCL_EXCEPTION__UNEXPECTED_RESPONSE_CODE = 0x08,
    TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY = 0x10,
    TRCL_EXCEPTION__FORBIDDEN_ACTION = 0x20,
    TRCL_EXCEPTION__ACCESS_DENIED = 0x40,
} TRCLExceptionCode;


typedef struct TRCLException TRCLException;


TRCLException * trcl_exception_alloc(
    TRCLExceptionCode,
    char const *
);
TRCLException * trcl_exception_copy_alloc(TRCLException const *);
TRCLException * trcl_exception_ok_alloc(void);
void trcl_exception_destroy(TRCLException *);

TRCLExceptionCode trcl_exception_get_code(TRCLException const *);
char const * trcl_exception_get_message(TRCLException const *);
void trcl_exception_set_message(TRCLException *, char const *);
void trcl_exception_set_own_message(TRCLException *, char const *);
void trcl_exception_set_ok(TRCLException *);

#endif
