#include "../subscribed-users-list.h"
#include "../../client.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


static size_t PER_PAGE = 30;


typedef struct TRCLResponseSubscribedUsersListDetailSuccess \
    ResponseDetailSuccess;
typedef struct TRCLResponseSubscribedUsersListDetail ResponseDetail;
typedef struct TRCLResponseSubscribedUsersList Response;


static ResponseDetailSuccess * success_response_from_json_alloc(
    json_t const*,
    json_error_t*
);
struct TRCLUserInfo * user_info_from_json_alloc(
    json_t const *,
    json_error_t*
);
static TRCLException * perform_request_alloc(
    struct TRCLClientConfig const *,
    size_t,
    json_t **,
    long int *
);


Response trcl_request_subscribed_users_list(
    struct TRCLClientConfig const * config,
    size_t page
) {
    Response response = {
        .exception=NULL,
        .detail=NULL,
    };

    json_t * json_response_content = NULL;
    long int response_status = -1;

    response.exception = perform_request_alloc(
        config,
        page,
        &json_response_content,
        &response_status
    );

    if (trcl_exception_get_code(response.exception)) {
        goto RequestError;
    }

    json_error_t json_error;
    if (response_status == 200) {
        ResponseDetailSuccess * detail = success_response_from_json_alloc(
            json_response_content,
            &json_error
        );
        response.detail = (ResponseDetail *) detail;

    } else {
        char message[128];
        sprintf(message, "Received code %li.", response_status);

        trcl_exception_destroy(response.exception);
        response.exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_CODE,
            message
        );
    }

    if (IS_NULL(response.detail)) {
        char message[128];
        sprintf(message, "Error parsing content.");

        trcl_exception_destroy(response.exception);
        response.exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNEXPECTED_RESPONSE_BODY,
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

ResponseDetailSuccess * success_response_from_json_alloc(
    json_t const * json_response_body,
    json_error_t * json_error
) {

    ResponseDetailSuccess * response = TRCL_ALLOC(ResponseDetailSuccess, 1);
    TRCL_ASSERT_ALLOC(response);
    response->is_last = TRUE;
    response->list = NULL;

    int max_page;
    json_t *results_json = NULL;

    int has_error = json_unpack_ex(
        (json_t *) json_response_body,
        json_error,
        JSON_STRICT,
        "{s:{s:i,s:o,*},*}",
        "data",
        "max_page",
        &max_page,
        "results",
        &results_json
    );

    if (has_error) {
        LOG(
            "Subscribed users list json error: on line %d: %s\n",
            json_error->line,
            json_error->text
        );
        TRCL_FREE(response);
        return NULL;
    }

    size_t results_count = 0;
    if (results_json != NULL && json_is_array(results_json)) {
        results_count = json_array_size(results_json);
    }
    response->list = trcl_model_list_user_info_alloc(results_count);
    response->is_last = results_count == 0;

    for (size_t i=0; i < results_count; i++) {
        json_t *user_info_json = json_array_get(results_json, i);
        struct TRCLUserInfo * user_info = user_info_from_json_alloc(
            user_info_json,
            json_error
        );
        if (IS_NULL(user_info)) {
            trcl_model_list_user_info_destroy(response->list);
            TRCL_FREE(response);
            return NULL;
        }
        trcl_model_user_info_move(&response->list->values[i], user_info);
        trcl_model_user_info_destroy(user_info);
    }

    return response;
};


struct TRCLUserInfo * user_info_from_json_alloc(
    json_t const * user_info_json,
    json_error_t * json_error
) {

    struct TRCLUserInfo * user_info = trcl_model_user_info_alloc();

    int is_active = FALSE;
    char const * user_id = NULL;
    char const * reference_id = NULL;
    json_t * provider = NULL;
    json_t * scopes = NULL;
    json_t * created_at = NULL;
    json_t * last_webhook_update_at = NULL;

    int has_error = json_unpack_ex(
        (json_t *) user_info_json,
        json_error,
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
        LOG(
            "User info json error: on line %d: %s\n",
            json_error->line,
            json_error->text
        );
        trcl_model_user_info_destroy(user_info);
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



TRCLException * perform_request_alloc(
    struct TRCLClientConfig const * config,
    size_t page,
    json_t ** json_response_content,
    long int * status_code
) {
    static char const path[] = "subscriptions";

    TRCLException * exception;

    CURL *curl = curl_easy_init();
    TRCL_ASSERT_ALLOC_OR(curl, {
        exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNKNOWN,
            "Error at curl_easy_init()"
        );
        goto ErrorCurl;
    });

    // base_url + path + "?per_page=XXXX&page=" + page
    size_t len_url = strlen(config->base_url) + strlen(path) + 20 + 12;
    char * url = str_alloc(len_url);

    snprintf(
        url,
        len_url + 1,
        "%s%s?per_page=%zu&page=%zu",
        config->base_url,
        path,
        PER_PAGE,
        page
    );

    curl_write_char_buffer_t buffer = {.data = NULL, .size=0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
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
