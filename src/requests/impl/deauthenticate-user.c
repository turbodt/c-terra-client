#include "../deauthenticate-user.h"
#include "../protected/shared.h"
#include "../../client.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


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
    ResponseProtected * response = trcl_response_alloc();

    json_t * json_content = NULL;

    response->exception = perform_request_alloc(
        config,
        user_id,
        &json_content,
        &response->status_code
    );

    if (trcl_exception_get_code(response->exception)) {
        goto RequestError;
    }

    if (response->status_code == 200) {
        // do nothing
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
