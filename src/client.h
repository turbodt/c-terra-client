#ifndef TRCL_CLIENT_H
#define TRCL_CLIENT_H


#include "./models.h"
#include "./exceptions.h"
#include "./config.h"


struct TRCLClientConfig {
    char const * api_key;
    char const * dev_id;
    char const * base_url;
};


struct TRCLClient {
    void (*destroy)(struct TRCLClient *);
    TRCLException const * (*get_last_exception)(struct TRCLClient const *);
    TRCLExceptionCode (*get_last_exception_code)(struct TRCLClient const *);
    struct TRCLUserInfo * (*get_user_info)(struct TRCLClient *, TRCLUserId);
    struct TRCLListUserInfo * (*get_subscribed_users_list)(struct TRCLClient *);
    void (*deauthenticate_user)(struct TRCLClient *, TRCLUserId);
};


struct TRCLClient * trcl_client_alloc(
    struct TRCLClientConfig const*,
    struct TRCLException **
);


#endif
