#include "../curl.h"
#include "../string.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


static char * build_query_string_alloc(char const * const *);


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


char * curl_uri_alloc(
    char const * base_url,
    char const * path,
    char const * const * queryParams
) {
    size_t len_base_url = strlen(base_url);
    size_t len_path = IS_NULL(path) ? 0 : strlen(path);

    while (len_base_url > 0 && base_url[len_base_url -1] == '/') {
        len_base_url--;
    }

    while (len_path > 0 && path[0] == '/') {
        path++;
        len_path--;
    }

    char * qstr = build_query_string_alloc(queryParams);
    size_t len_qstr = IS_NULL(qstr) ? 0 : strlen(qstr);

    size_t len_uri = len_base_url;
    if (len_path > 0) {
        len_uri += 1 + len_path;
    }
    if (len_qstr > 0) {
        len_uri += 1 + len_qstr;
    }

    char * uri = str_alloc(len_uri);
    strncpy(uri, base_url, len_base_url);
    uri[len_base_url] = '\0';
    if (len_path > 0) {
        strncat(uri, "/", 1 + 1);
        strncat(uri, path, len_path + 1);
    }
    if (len_qstr > 0) {
        strncat(uri, "?", 1 + 1);
        strncat(uri, qstr, len_qstr + 1);
    }
    str_destroy(qstr);

    return uri;
};


size_t curl_response_write_text(
    void *content,
    size_t size,
    size_t nmemb,
    void *stream
) {

    size_t realsize = size * nmemb;
    curl_write_char_buffer_t *mem = (curl_write_char_buffer_t *)stream;

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

    curl_write_char_buffer_t buffer = {.data = NULL, .size=0};
    size_t realsize = curl_response_write_text(content, size, nmemb, &buffer);

    json_t **root = (json_t **) stream;
    json_error_t error;

    *root = json_loads(buffer.data, 0, &error);
    if(IS_NULL(*root)) {
        LOG("json error: on line %d: %s\n", error.line, error.text);
        LOG("readed: %zu\n", realsize);
        LOG("nmemb: %zu\n", nmemb);
        LOG("json source:\n%s\n", buffer.data);
        goto ErrorParsingJson;
    }

ErrorParsingJson:
    TRCL_FREE(buffer.data);
    return realsize;
};


char * build_query_string_alloc(char const * const *qp) {
    if (IS_NULL(qp) || IS_NULL(*qp)) {
        return NULL;
    }

    char const * const * it = qp;

    size_t len_qstr = 0;
    while (!IS_NULL(*it)) {
        len_qstr += strlen(*it);
        it++;
        len_qstr++;
    }
    len_qstr--;

    it = qp;
    size_t offset = 0;
    char * qstr = str_alloc(len_qstr);
    while (!IS_NULL(*it)) {

        if (it != qp) {
            qstr[offset] = '&';
            offset++;
        }

        size_t len = strlen(*it);
        strncpy(qstr + offset, *it, len);
        offset += len;
        it++;

        qstr[offset] = '=';
        offset++;

        len = strlen(*it);
        strncpy(qstr + offset, *it, len);
        offset += len;
        it++;
    }
    return qstr;
};
