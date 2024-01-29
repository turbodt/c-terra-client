#include "../user-info.h"
#include "../../client.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


typedef struct TRCLResponseUserInfoDetailSuccess SuccessResponse;


static SuccessResponse * success_response_from_json_alloc(json_t const*);
static TRCLException * perform_request_alloc(
    struct TRCLClientConfig const *,
    TRCLUserId,
    json_t **,
    long int *
);


struct TRCLResponseUserInfo trcl_request_user_info(
    struct TRCLClientConfig const * config,
    TRCLUserId user_id
) {
    struct TRCLResponseUserInfo response = {
        .exception=NULL,
        .detail=NULL,
    };

    json_t * json_response_content = NULL;
    long int response_status = -1;

    response.exception = perform_request_alloc(
        config,
        user_id,
        &json_response_content,
        &response_status
    );

    if (trcl_exception_get_code(response.exception)) {
        goto RequestError;
    }

    if (response_status == 200) {
        SuccessResponse * detail = success_response_from_json_alloc(
            json_response_content
        );
        response.detail = (struct TRCLResponseUserInfoDetail *) detail;

    } else {
        char message[128];
        sprintf(message, "Received code %li.", response_status);

        trcl_exception_destroy(response.exception);
        response.exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_CODE,
            message
        );
    }

RequestError:
    json_decref(json_response_content);
    return response;
};


//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------

//{"is_authenticated":true,"status":"success","user":{"active":true,"created_at":"2024-01-26T12:53:08.426703+00:00","last_webhook_update":"2024-01-29T07:23:08.860546+00:00","provider":"WITHINGS","reference_id":"3","scopes":"user.activity,user.metrics","user_id":"aacab2e7-610f-4714-b309-3b4ff2cf04dd"}}
SuccessResponse * success_response_from_json_alloc(
    json_t const * json_response_body
) {

    SuccessResponse * response = TRCL_ALLOC(SuccessResponse, 1);
    TRCL_ASSERT_ALLOC(response);

    int is_active = FALSE;
    char const * user_id = NULL;
    char const * reference_id = NULL;
    char const * provider = NULL;
    char const * scopes = NULL;
    char const * created_at = NULL;
    char const * last_webhook_update_at = NULL;

    json_error_t json_error;
    int has_error = json_unpack_ex(
        (json_t *) json_response_body,
        &json_error,
        JSON_STRICT,
        "{s:{s:b, s:s, s:s, s:s, s:s, s:s, s:s, *},*}",
        "user",
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
        LOG(
            "json error: on line %d: %s",
            json_error.line,
            json_error.text
        );
        goto ErrorParsingField;
    }

    response->is_active = is_active;
    response->user_id = str_copy_alloc(user_id);
    response->reference_id = str_copy_alloc(reference_id);
    response->provider = str_copy_alloc(provider);
    response->scopes = str_copy_alloc(scopes);
    response->created_at = str_copy_alloc(created_at);
    response->last_webhook_update_at = str_copy_alloc(last_webhook_update_at);

    return response;

ErrorParsingField:
    TRCL_FREE(response);
    return NULL;
};


TRCLException * perform_request_alloc(
    struct TRCLClientConfig const * config,
    TRCLUserId user_id,
    json_t ** json_response_content,
    long int * status_code
) {
    static char const path[] = "userInfo";

    TRCLException * exception;

    CURL *curl = curl_easy_init();
    TRCL_ASSERT_ALLOC_OR(curl, {
        exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNKNOWN,
            "Error at curl_easy_init()"
        );
        goto ErrorCurl;
    });

    // base_url + path + "?" + "user_id" + user_id
    size_t len_url = strlen(config->base_url) + strlen(path) + 1 + 8 + 36;
    char * url = str_alloc(len_url);

    snprintf(url, len_url + 1, "%s%s?user_id=%s", config->base_url, path, user_id);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_response_write_json);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, json_response_content);

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

    exception = trcl_exception_ok_alloc();

ErrorUnablePerformRequest:
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    str_destroy(url);
ErrorCurl:
    return exception;
};
