#ifndef TRCL_SHARED_CURL_H
#define TRCL_SHARED_CURL_H


#include <stddef.h>


struct curl_slist * curl_slist_append_kv(
    struct curl_slist *,
    char const *,
    char const *
);
size_t curl_response_write_text(void *, size_t, size_t, void *);
size_t curl_response_write_json(void *, size_t, size_t, void *);


#endif
