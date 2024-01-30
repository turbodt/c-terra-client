#include "../protected/client.h"
#include "../requests/user-info.h"
#include "../models.h"
#include "../base.h"
#include <string.h>


struct TRCLUserInfo * trcl_client_get_user_info(
    Client *client,
    TRCLUserId user_id
) {
    struct TRCLUserInfo * user_info = trcl_model_user_info_alloc();

    ClientProtected * pclient = (ClientProtected *) client;
    struct TRCLResponseUserInfo response = trcl_request_user_info(
        &pclient->config,
        user_id
    );

    pclient->set_own_exception(pclient, response.exception);

    if (trcl_exception_get_code(response.exception)) {
        goto RequestError;
    }

    struct TRCLResponseUserInfoDetailSuccess * detail = \
        (struct TRCLResponseUserInfoDetailSuccess *) response.detail;

    user_info->is_active = detail->is_active;
    user_info->user_id = detail->user_id;
    user_info->reference_id = detail->reference_id;
    user_info->provider = detail->provider;
    user_info->created_at = detail->created_at;
    user_info->last_webhook_update_at = detail->last_webhook_update_at;
    user_info->scopes = detail->scopes;

RequestError:
    TRCL_FREE(response.detail);
    return user_info;
};
