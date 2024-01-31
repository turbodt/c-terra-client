#include "../protected/client.h"
#include "../requests/user-info.h"
#include "../models.h"
#include "../base.h"
#include "../shared/string.h"
#include <string.h>


struct TRCLUserInfo * trcl_client_get_user_info(
    Client *client,
    TRCLUserId user_id
) {
    struct TRCLUserInfo * user_info = trcl_model_user_info_alloc();

    ClientProtected * pclient = (ClientProtected *) client;
    struct TRCLResponse * response = trcl_request_user_info(
        &pclient->config,
        user_id
    );

    pclient->set_exception(pclient, response->get_exception(response));

    if (trcl_exception_get_code(response->get_exception(response))) {
        goto RequestError;
    }

    struct TRCLResponseUserInfoBodySuccess const * body = \
        response->get_body(response);

    user_info->is_active = body->is_active;
    user_info->user_id = str_copy_alloc(body->user_id);
    user_info->reference_id = str_copy_alloc(body->reference_id);
    user_info->provider = str_copy_alloc(body->provider);
    user_info->created_at = str_copy_alloc(body->created_at);
    user_info->last_webhook_update_at = str_copy_alloc(
        body->last_webhook_update_at
    );
    user_info->scopes = str_copy_alloc(body->scopes);

RequestError:
    response->destroy(response);
    return user_info;
};
