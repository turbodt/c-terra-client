#include "../subscribed-users-list.h"
#include "../protected/user-info.h"
#include "../protected/shared.h"
#include "../../client.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


typedef struct TRCLResponseSubscribedUsersListBodySuccess SuccessBody;
typedef struct CustomResponse {
    ResponseProtected base;
    void (*base_destroy)(Response *);
} CustomResponse;

static ResponseProtected * custom_response_alloc(void);
static void custom_response_destroy(Response *);
static SuccessBody * body_success_alloc(json_t const*, struct TRCLException **);
static void body_success_destroy(SuccessBody *);


Response * trcl_request_subscribed_users_list(
    struct TRCLClientConfig const * config,
    size_t page
) {
    ResponseProtected * response = custom_response_alloc();

    json_t * json_content = NULL;

    char page_str[21];
    snprintf(page_str, sizeof(page_str), "%zu", page);
    response->exception = trcl_perform_request_alloc(
        config,
        &json_content,
        &response->status_code,
        "GET",
        "/subscriptions",
        (char const * const []) {
            "page",
            page_str,
            "per_page",
            TRCL_REQUEST_PER_PAGE,
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
        sprintf(message, "Received code %li.", response->status_code);

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

    SuccessBody * body_success = TRCL_ALLOC(SuccessBody, 1);
    TRCL_ASSERT_ALLOC(body_success);
    body_success->is_last = TRUE;
    body_success->list = NULL;

    int max_page;
    json_t *results_json = NULL;

    json_error_t json_error;
    int has_error = json_unpack_ex(
        (json_t *) json_response_body,
        &json_error,
        JSON_STRICT,
        "{s:{s:i,s:o,*},*}",
        "data",
        "max_page",
        &max_page,
        "results",
        &results_json
    );

    if (has_error) {
        trcl_exception_destroy(*exception);
        char * message = trcl_json_error_to_str_alloc(&json_error);
        *exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
            NULL
        );
        trcl_exception_set_own_message(*exception, message);
        TRCL_FREE(body_success);
        return NULL;
    }

    size_t results_count = 0;
    if (results_json != NULL && json_is_array(results_json)) {
        results_count = json_array_size(results_json);
    }
    body_success->list = trcl_model_list_user_info_alloc(results_count);
    body_success->is_last = results_count == 0;

    for (size_t i=0; i < results_count; i++) {
        json_t *user_info_json = json_array_get(results_json, i);
        struct TRCLUserInfo * user_info = user_info_from_json_alloc(
            user_info_json,
            exception
        );
        if (IS_NULL(user_info)) {
            body_success_destroy(body_success);
            return NULL;
        }
        trcl_model_user_info_move(&body_success->list->values[i], user_info);
        trcl_model_user_info_destroy(user_info);
    }

    return body_success;
};


void body_success_destroy(SuccessBody * success_body) {
    RETURN_IF_NULL(success_body);
    trcl_model_list_user_info_destroy(success_body->list);
    TRCL_FREE(success_body);
};
