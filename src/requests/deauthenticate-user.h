#ifndef TRCL_REQUESTS_DEAUTHENTICATE_USER_H
#define TRCL_REQUESTS_DEAUTHENTICATE_USER_H


#include "./shared.h"
#include "../models.h"
#include "../client.h"
#include "../config.h"


struct TRCLResponse * trcl_request_deauthenticate_user(
    struct TRCLClientConfig const *,
    TRCLUserId
);


#endif
