#include "../protected/shared.h"
#include "../../base.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


TRCLException * trcl_perform_request_alloc(
    struct TRCLClientConfig const * config,
    json_t ** json_response_content,
    long int * status_code,
    char const * method,
    char const * path,
    char const * const * query_params
) {

    TRCLException * exception;

    CURL *curl = curl_easy_init();
    TRCL_ASSERT_ALLOC_OR(curl, {
        exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNKNOWN,
            "Error at curl_easy_init()"
        );
        goto ErrorCurl;
    });

    char * url = curl_uri_alloc(
        config->base_url,
        path,
        query_params
    );
    curl_write_char_buffer_t buffer = {.data = NULL, .size=0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_response_write_text);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &buffer);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append_kv(headers, "accept", "application/json");
    headers = curl_slist_append_kv(headers, "dev-id", config->dev_id);
    headers = curl_slist_append_kv(headers, "x-api-key", config->api_key);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        char message[1024];
        sprintf(
            message,
            "Error performing request to `%s`:\n%s\n",
            url,
            curl_easy_strerror(res)
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
        exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
            NULL
        );
        char * message = trcl_json_error_to_str_alloc(&error);
        trcl_exception_set_own_message(exception, message);
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
