#ifndef TRCL_REQUESTS_USER_INFO_H
#define TRCL_REQUESTS_USER_INFO_H


#include "./shared.h"
#include "../models.h"
#include "../client.h"
#include "../config.h"


struct TRCLResponseUserInfoDetail {};

struct TRCLResponseUserInfoDetailSuccess {
    struct TRCLResponseUserInfoDetail base;
    TRCLUserId user_id;
    TRCLReferenceId reference_id;
    TRCL_BOOL is_active;
    TRCLTimestamp created_at;
    TRCLTimestamp last_webhook_update_at;
    TRCLProvider provider;
    TRCLUserScopes scopes;
};


struct TRCLResponseUserInfo {
    struct TRCLException * exception;
    //enum TRCLResponseStatus status;
    //TRCL_BOOL is_authenticated;
    struct TRCLResponseUserInfoDetail * detail;
};


struct TRCLResponseUserInfo trcl_request_user_info(
    struct TRCLClientConfig const *,
    TRCLUserId
);

#endif
