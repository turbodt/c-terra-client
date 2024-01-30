#include "../protected/client.h"
#include "../requests/subscribed-users-list.h"
#include "../models.h"
#include "../base.h"


struct TRCLListUserInfo * trcl_client_get_subscribed_users_list(
    Client *client
) {
    ClientProtected * pclient = (ClientProtected *) client;

    struct TRCLListUserInfo * user_list = trcl_model_list_user_info_alloc(0);

    TRCL_BOOL is_last = FALSE;
    for (size_t page = 0; !is_last; page++) {
        struct TRCLResponseSubscribedUsersList response;
        response = trcl_request_subscribed_users_list(&pclient->config, page);

        pclient->set_own_exception(pclient, response.exception);

        if (trcl_exception_get_code(response.exception)) {
            trcl_model_list_user_info_destroy(user_list);
            user_list = NULL;
            is_last = TRUE;
            goto RequestError;
        }

        struct TRCLResponseSubscribedUsersListDetailSuccess * detail = \
            (struct TRCLResponseSubscribedUsersListDetailSuccess *) response.detail;

        trcl_model_list_user_info_own_concat(user_list, detail->list);
        detail->list = NULL;

        is_last = detail->is_last;
RequestError:
        TRCL_FREE(response.detail);
    }

    return user_list;
};
