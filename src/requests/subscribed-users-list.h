#ifndef TRCL_REQUESTS_SUBSCRIBED_USERS_LIST_H
#define TRCL_REQUESTS_SUBSCRIBED_USERS_LIST_H


#include "./shared.h"
#include "../models.h"
#include "../client.h"
#include "../config.h"



struct TRCLResponseSubscribedUsersListBodySuccess {
    struct TRCLListUserInfo * list;
    TRCL_BOOL is_last;
};


struct TRCLResponse * trcl_request_subscribed_users_list(
    struct TRCLClientConfig const *,
    size_t
);


#endif
