#include "../deauthenticate-user.h"
#include "../../client.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


typedef struct TRCLResponseDeauthenticateUserDetailSuccess \
    ResponseDetailSuccess;
typedef struct TRCLResponseDeauthenticateUserDetailFail \
    ResponseDetailFail;
typedef struct TRCLResponseDeauthenticateUserDetail ResponseDetail;
typedef struct TRCLResponseDeauthenticateUser Response;


static ResponseDetailSuccess * success_response_from_json_alloc(
    json_t const*,
    json_error_t*
);
static ResponseDetailFail * fail_response_from_json_alloc(
    json_t const*,
    json_error_t*
);
struct TRCLUserInfo * user_info_from_json_alloc(
    json_t const *,
    json_error_t*
);
static TRCLException * perform_request_alloc(
    struct TRCLClientConfig const *,
    TRCLUserId,
    json_t **,
    long int *
);


Response * trcl_request_deauthenticate_user(
    struct TRCLClientConfig const * config,
    TRCLUserId user_id
) {
    Response * response = TRCL_ALLOC(Response, 1);
    response->exception = NULL;
    response->detail = NULL;

    json_t * json_response_content = NULL;
    long int response_status = -1;

    response->exception = perform_request_alloc(
        config,
        user_id,
        &json_response_content,
        &response_status
    );

    if (trcl_exception_get_code(response->exception)) {
        goto RequestError;
    }

    json_error_t json_error;
    if (response_status == 200) {
        ResponseDetailSuccess * detail = success_response_from_json_alloc(
            json_response_content,
            &json_error
        );
        response->detail = (ResponseDetail *) detail;

    } else if (response_status == 400) {
        ResponseDetailFail * detail = fail_response_from_json_alloc(
            json_response_content,
            &json_error
        );
        response->detail = (ResponseDetail *) detail;
        trcl_exception_destroy(response->exception);
        response->exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_CODE,
            detail->message
        );
    } else if (response_status == 404) {
        ResponseDetailFail * detail = fail_response_from_json_alloc(
            json_response_content,
            &json_error
        );
        response->detail = (ResponseDetail *) detail;
        trcl_exception_destroy(response->exception);
        response->exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_CODE,
            detail->message
        );
    } else {
        char message[128];
        sprintf(message, "Received code %li.", response_status);

        trcl_exception_destroy(response->exception);
        response->exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_CODE,
            message
        );
    }

    if (IS_NULL(response->detail)) {
        char message[128];
        sprintf(message, "Error parsing content.");

        trcl_exception_destroy(response->exception);
        response->exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
            message
        );
    }

RequestError:
    json_decref(json_response_content);
    return response;
};


void trcl_response_deauthenticate_user_destroy(
    struct TRCLResponseDeauthenticateUser * response
) {
    if (response->detail->status == TRCL_RESPONSE_STATUS__FAIL) {
        ResponseDetailFail * detail = (ResponseDetailFail *) response->detail;
        str_destroy(detail->message);
    }
    TRCL_FREE(response->detail);
    trcl_exception_destroy(response->exception);
    TRCL_FREE(response);
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------


ResponseDetailSuccess * success_response_from_json_alloc(
    json_t const * json_response_body,
    json_error_t * json_error
) {

    ResponseDetailSuccess * response = TRCL_ALLOC(ResponseDetailSuccess, 1);
    TRCL_ASSERT_ALLOC(response);
    response->base.status = TRCL_RESPONSE_STATUS__FAIL;

    const char * status;

    int has_error = json_unpack_ex(
        (json_t *) json_response_body,
        json_error,
        JSON_STRICT,
        "{s:s}",
        "status",
        &status
    );

    if (has_error) {
        LOG(
            "Deauthenticate user json error: on line %d: %s\n",
            json_error->line,
            json_error->text
        );
        TRCL_FREE(response);
        return NULL;
    }

    if (strcmp("success", status) == 0) {
        response->base.status = TRCL_RESPONSE_STATUS__SUCCESS;
    }

    return response;
};


ResponseDetailFail * fail_response_from_json_alloc(
    json_t const * json_response_body,
    json_error_t * json_error
) {

    ResponseDetailFail * response = TRCL_ALLOC(ResponseDetailFail, 1);
    TRCL_ASSERT_ALLOC(response);
    response->base.status = TRCL_RESPONSE_STATUS__FAIL;

    const char * status;
    const char * message;

    int has_error = json_unpack_ex(
        (json_t *) json_response_body,
        json_error,
        JSON_STRICT,
        "{s:s,s:s}",
        "status",
        &status,
        "message",
        &message
    );

    if (has_error) {
        LOG(
            "Deauthenticate user json error: on line %d: %s\n",
            json_error->line,
            json_error->text
        );
        TRCL_FREE(response);
        return NULL;
    }

    response->message = str_copy_alloc(message);

    return response;
};


TRCLException * perform_request_alloc(
    struct TRCLClientConfig const * config,
    TRCLUserId user_id,
    json_t ** json_response_content,
    long int * status_code
) {
    static char const path[] = "auth/deauthenticateUser";

    TRCLException * exception;

    CURL *curl = curl_easy_init();
    TRCL_ASSERT_ALLOC_OR(curl, {
        exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNKNOWN,
            "Error at curl_easy_init()"
        );
        goto ErrorCurl;
    });

    // base_url + path + "?user_id=" + user_id
    size_t len_url = strlen(config->base_url) + strlen(path) + 9 + strlen(user_id);
    char * url = str_alloc(len_url);

    snprintf(
        url,
        len_url + 1,
        "%s%s?user_id=%s",
        config->base_url,
        path,
        user_id
    );

    curl_write_char_buffer_t buffer = {.data = NULL, .size=0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_response_write_text);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &buffer);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append_kv(headers, "accept", "application/json");
    headers = curl_slist_append_kv(headers, "dev-id", config->dev_id);
    headers = curl_slist_append_kv(headers, "x-api-key", config->api_key);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode status = curl_easy_perform(curl);
    if (status != 0) {
        char message[1024];
        sprintf(
            message,
            "Error performing request to `%s`:\n%s\n",
            url,
            curl_easy_strerror(status)
        );
        exception = trcl_exception_alloc(
            TRCL_EXCEPTION__CANNOT_REQUEST,
            message
        );
        goto ErrorUnablePerformRequest;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, status_code);

    json_error_t error;
    *json_response_content = json_loads(buffer.data, 0, &error);
    if(IS_NULL(*json_response_content)) {
        char message[1024];
        sprintf(
            message,
            "Error performing request to `%s`:\n%s\n",
            url,
            curl_easy_strerror(status)
        );
        exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
            message
        );
        goto ErrorParsingCurlJson;
    }
    exception = trcl_exception_ok_alloc();

ErrorParsingCurlJson:
    TRCL_FREE(buffer.data);
ErrorUnablePerformRequest:
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    str_destroy(url);
ErrorCurl:
    return exception;
};
