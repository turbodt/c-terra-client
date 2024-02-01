#include "../user-info.h"
#include "../protected/user-info.h"
#include "../protected/shared.h"
#include "../../client.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


typedef struct TRCLResponseUserInfoBodySuccess SuccessBody;
typedef struct CustomResponse {
    ResponseProtected base;
    void (*base_destroy)(Response *);
} CustomResponse;

static ResponseProtected * custom_response_alloc(void);
static void custom_response_destroy(Response *);
static SuccessBody * body_success_alloc(json_t const*, struct TRCLException **);
static void body_success_destroy(SuccessBody *);


Response * trcl_request_user_info(
    struct TRCLClientConfig const * config,
    TRCLUserId user_id
) {
    ResponseProtected * response = custom_response_alloc();

    json_t * json_content = NULL;

    response->exception = trcl_perform_request_alloc(
        config,
        &json_content,
        &response->status_code,
        "GET",
        "/userInfo",
        (char const * const []) {
            "user_id",
            user_id,
            NULL
        }
    );

    if (trcl_exception_get_code(response->exception)) {
        goto RequestError;
    }

    if (response->status_code == 200) {
        response->body = body_success_alloc(json_content, &response->exception);
    } else if (response->status_code == 400) {
        response->body = body_fail_alloc(json_content, &response->exception);
    } else if (response->status_code == 404) {
        response->body = body_fail_alloc(json_content, &response->exception);
    } else {
        char message[128];
        sprintf(message, "Received code %ld.", response->status_code);

        trcl_exception_destroy(response->exception);
        response->exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_CODE,
            message
        );
    }

RequestError:
    json_decref(json_content);
    return (Response *) response;
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


ResponseProtected * custom_response_alloc(void) {
    CustomResponse * response = TRCL_ALLOC(CustomResponse, 1);
    TRCL_ASSERT_ALLOC(response);
    trcl_response_init(&response->base);
    response->base_destroy = response->base.base.destroy;
    response->base.base.destroy = custom_response_destroy;
    return (ResponseProtected *) response;
};


void custom_response_destroy(Response * response) {
    ResponseProtected * presponse = (ResponseProtected *) response;
    long int status_code = response->get_status_code(response);
    if (status_code == 200) {
        body_success_destroy(presponse->body);
    } else if (status_code == 404 || status_code == 400) {
        body_fail_destroy(presponse->body);
    }
    ((CustomResponse *) response)->base_destroy(response);
};

SuccessBody * body_success_alloc(
    json_t const * json_response_body,
    struct TRCLException **exception
) {

    SuccessBody * success_body = TRCL_ALLOC(SuccessBody, 1);
    TRCL_ASSERT_ALLOC(success_body);

    json_t *user_info_json = NULL;

    json_error_t json_error;
    int has_error = json_unpack_ex(
        (json_t *) json_response_body,
        &json_error,
        JSON_STRICT,
        "{s:o,*}",
        "user",
        &user_info_json
    );

    if (has_error) {
        trcl_exception_destroy(*exception);
        char * message = trcl_json_error_to_str_alloc(&json_error);
        *exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
            NULL
        );
        trcl_exception_set_own_message(*exception, message);
        TRCL_FREE(success_body);
        return NULL;
    }

    struct TRCLUserInfo * user_info = user_info_from_json_alloc(
        user_info_json,
        exception
    );
    if (IS_NULL(user_info)) {
        body_success_destroy(success_body);
        return NULL;
    }

    success_body->is_active = user_info->is_active;
    success_body->user_id = user_info->user_id;
    user_info->user_id = NULL;
    success_body->reference_id = user_info->reference_id;
    user_info->reference_id = NULL;
    success_body->provider = user_info->provider;
    user_info->provider = NULL;
    success_body->scopes = user_info->scopes;
    user_info->scopes = NULL;
    success_body->created_at = user_info->created_at;
    user_info->created_at = NULL;
    success_body->last_webhook_update_at = user_info->last_webhook_update_at;
    user_info->last_webhook_update_at = NULL;

    trcl_model_user_info_destroy(user_info);

    return success_body;
};


static void body_success_destroy(SuccessBody *success_body) {
    RETURN_IF_NULL(success_body);
    str_destroy(success_body->user_id);
    str_destroy(success_body->reference_id);
    str_destroy(success_body->provider);
    str_destroy(success_body->scopes);
    str_destroy(success_body->created_at);
    str_destroy(success_body->last_webhook_update_at);
    TRCL_FREE(success_body);
};


struct TRCLUserInfo * user_info_from_json_alloc(
    json_t const * user_info_json,
    struct TRCLException **exception
) {

    struct TRCLUserInfo * user_info = trcl_model_user_info_alloc();

    int is_active = FALSE;
    char const * user_id = NULL;
    char const * reference_id = NULL;
    json_t * provider = NULL;
    json_t * scopes = NULL;
    json_t * created_at = NULL;
    json_t * last_webhook_update_at = NULL;

    json_error_t json_error;
    int has_error = json_unpack_ex(
        (json_t *) user_info_json,
        &json_error,
        JSON_STRICT,
        "{s:b, s:s, s:s, s:o, s:o, s:o, s:o, *}",
        "active",
        &is_active,
        "user_id",
        &user_id,
        "reference_id",
        &reference_id,
        "provider",
        &provider,
        "scopes",
        &scopes,
        "created_at",
        &created_at,
        "last_webhook_update",
        &last_webhook_update_at
    );

    if (has_error) {
        trcl_exception_destroy(*exception);
        char * message = trcl_json_error_to_str_alloc(&json_error);
        *exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
            NULL
        );
        trcl_exception_set_own_message(*exception, message);
        TRCL_FREE(user_info);
        return NULL;
    }

    user_info->is_active = is_active;
    user_info->user_id = str_copy_alloc(user_id);
    user_info->reference_id = str_copy_alloc(reference_id);
    user_info->provider = str_copy_alloc(json_string_value(provider));
    user_info->scopes = str_copy_alloc(json_string_value(scopes));
    user_info->created_at = str_copy_alloc(json_string_value(created_at));
    user_info->last_webhook_update_at = str_copy_alloc(
        json_string_value(last_webhook_update_at)
    );

    return user_info;
};
