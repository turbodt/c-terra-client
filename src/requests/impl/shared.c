#include "../protected/shared.h"
#include "../../base.h"
#include "../../exceptions.h"
#include "../../shared/string.h"
#include <jansson.h>


static void trcl_response_destroy(Response *);
static struct TRCLException const * trcl_response_get_exception(
    Response const *
);
static long int trcl_response_get_status_code(Response const *);
static void const * trcl_response_get_body(Response const *);


ResponseProtected * trcl_response_alloc(void) {
    ResponseProtected * response = TRCL_ALLOC(ResponseProtected, 1);
    TRCL_ASSERT_ALLOC(response);
    trcl_response_init(response);
    return response;
};


void trcl_response_init(ResponseProtected * response) {
    response->exception = NULL;
    response->status_code = -1;
    response->body = NULL;
    response->base = (struct TRCLResponse){
        .destroy = trcl_response_destroy,
        .get_exception = trcl_response_get_exception,
        .get_status_code = trcl_response_get_status_code,
        .get_body = trcl_response_get_body,
    };
};


char * trcl_json_error_to_str_alloc(json_error_t const *json_error) {
    size_t message_len = 128;
    char * message = str_alloc(message_len);
    snprintf(
        message,
        message_len,
        "Json error on line %d:%d. \"%s\". Source: \"%s\"\n",
        json_error->line,
        json_error->column,
        json_error->text,
        json_error->source
    );
    return message;
};


BodyFail * body_fail_alloc(
    json_t const* json_content,
    struct TRCLException ** exception
) {
    BodyFail * body = TRCL_ALLOC(BodyFail, 1);
    TRCL_ASSERT_ALLOC(body);

    const char * status;
    const char * message;
    json_error_t json_error;

    int has_error = json_unpack_ex(
        (json_t *) json_content,
        &json_error,
        JSON_STRICT,
        "{s:s,s:s}",
        "status",
        &status,
        "message",
        &message
    );

    if (has_error) {
        trcl_exception_destroy(*exception);
        char * message = trcl_json_error_to_str_alloc(&json_error);
        *exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
            NULL
        );
        trcl_exception_set_own_message(*exception, message);
        TRCL_FREE(body);
        return NULL;
    }

    body->message = str_copy_alloc(message);
    return body;
};


void body_fail_destroy(BodyFail *body) {
    RETURN_IF_NULL(body);
    str_destroy(body->message);
    TRCL_FREE(body);
};


void trcl_response_destroy(Response * response) {
    ResponseProtected * presponse = (ResponseProtected *) response;
    trcl_exception_destroy(presponse->exception);
    TRCL_FREE(presponse);
};


inline struct TRCLException const * trcl_response_get_exception(
    Response const * response
) {
    return ((ResponseProtected const *) response)->exception;
};


inline long int trcl_response_get_status_code(Response const *response) {
    return ((ResponseProtected const *) response)->status_code;
};


inline void const * trcl_response_get_body(Response const *response) {
    return ((ResponseProtected const *) response)->body;
};
