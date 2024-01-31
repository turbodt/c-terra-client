#ifndef TRCL_REQUESTS_DEAUTHENTICATE_USER_H
#define TRCL_REQUESTS_DEAUTHENTICATE_USER_H


#include "./shared.h"
#include "../models.h"
#include "../client.h"
#include "../config.h"


struct TRCLResponseDeauthenticateUserDetail {
    enum TRCLResponseStatus status;
};

struct TRCLResponseDeauthenticateUserDetailSuccess {
    struct TRCLResponseDeauthenticateUserDetail base;
};

struct TRCLResponseDeauthenticateUserDetailFail {
    struct TRCLResponseDeauthenticateUserDetail base;
    char const * message;
};


struct TRCLResponseDeauthenticateUser {
    struct TRCLException * exception;
    struct TRCLResponseDeauthenticateUserDetail * detail;
};


struct TRCLResponseDeauthenticateUser * trcl_request_deauthenticate_user(
    struct TRCLClientConfig const *,
    TRCLUserId
);


void trcl_response_deauthenticate_user_destroy(
    struct TRCLResponseDeauthenticateUser *
);


#endif
