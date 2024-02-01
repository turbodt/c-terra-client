#include "../deauthenticate-user.h"
#include "../protected/shared.h"
#include "../../client.h"
#include "../../shared/string.h"
#include "../../shared/curl.h"
#include "../../base.h"
#include <jansson.h>
#include <curl/curl.h>
#include <string.h>


Response * trcl_request_deauthenticate_user(
    struct TRCLClientConfig const * config,
    TRCLUserId user_id
) {
    ResponseProtected * response = trcl_response_alloc();

    json_t * json_content = NULL;


    response->exception = trcl_perform_request_alloc(
        config,
        &json_content,
        &response->status_code,
        "DELETE",
        "/auth/deauthenticateUser",
        (char const * const []) {
            "user_id",
            user_id,
            NULL
        }
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
