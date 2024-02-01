#ifndef TRCL_REQUESTS_PROTECTED_SHARED_H
#define TRCL_REQUESTS_PROTECTED_SHARED_H


#include "../shared.h"
#include "../../exceptions.h"
#include "../../client.h"
#include <jansson.h>


typedef struct TRCLResponse Response;
typedef struct TRCLResponseBodyFail BodyFail;


typedef enum ResponseType ResponseType;


typedef struct ResponseProtected {
    Response base;
    struct TRCLException * exception;
    long int status_code;
    void * body;
} ResponseProtected;


ResponseProtected * trcl_response_alloc(void);
void trcl_response_init(ResponseProtected *);
char * trcl_json_error_to_str_alloc(json_error_t const *);
BodyFail * body_fail_alloc(json_t const*, struct TRCLException **);
void body_fail_destroy(BodyFail *);


TRCLException * trcl_perform_request_alloc(
    struct TRCLClientConfig const *,
    json_t **,
    long int *,
    char const *,
    char const *,
    char const * const *
);


#endif
