#ifndef TRCL_SHARED_CURL_H
#define TRCL_SHARED_CURL_H


#include <stddef.h>


typedef struct curl_write_char_buffer_t {
    char *data;
    size_t size;
} curl_write_char_buffer_t;


struct curl_slist * curl_slist_append_kv(
    struct curl_slist *,
    char const *,
    char const *
);
char * curl_uri_alloc(
    char const *,
    char const *,
    char const * const *
);
size_t curl_response_write_text(void *, size_t, size_t, void *);
size_t curl_response_write_json(void *, size_t, size_t, void *);


#endif
