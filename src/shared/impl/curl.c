#include "../curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


struct curl_slist * curl_slist_append_kv(
    struct curl_slist * headers,
    char const *key,
    char const *value
) {
    size_t len = strlen(key) + 2 + strlen(value) + 1;
    char * header = TRCL_ALLOC(char, len * sizeof(char));
    snprintf(header, len, "%s: %s", key, value);
    struct curl_slist * new_headers = curl_slist_append(headers, header);
    TRCL_FREE(header);
    return new_headers;
};


size_t curl_response_write_text(
    void *content,
    size_t size,
    size_t nmemb,
    void *stream
) {

    struct write_result_t {
        char *data;
        int size;
    };

    size_t realsize = size * nmemb;
    struct write_result_t *mem = (struct write_result_t *)stream;

    mem->data = TRCL_REALLOC(mem->data, char, mem->size + realsize + 1);

    memcpy(&(mem->data[mem->size]), content, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
};


size_t curl_response_write_json(
    void *content,
    size_t size,
    size_t nmemb,
    void *stream
) {

    struct write_result_t {
        char *data;
        int size;
    };

    struct write_result_t result_text = {.data = NULL, .size=0};
    size_t realsize = curl_response_write_text(content, size, nmemb, &result_text);

    json_t **root = (json_t **) stream;
    json_error_t error;

    *root = json_loads(result_text.data, 0, &error);
    if(IS_NULL(*root)) {
        LOG("json error: on line %d: %s", error.line, error.text);
        LOG("readed: %zu", realsize);
        LOG("json source:\n%s", result_text.data);
        goto ErrorParsingJson;
    }

ErrorParsingJson:
    TRCL_FREE(result_text.data);
    return realsize;
};
