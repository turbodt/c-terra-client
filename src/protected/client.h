#ifndef TRCL_PROTECTED_CLIENT_H
#define TRCL_PROTECTED_CLIENT_H


#include "../client.h"


typedef struct TRCLClient Client;
typedef struct TRCLClientConfig ClientConfig;


typedef struct ClientProtected {
    Client base;
    ClientConfig config;
    TRCLException * last_exception;
    struct ClientProtected * (*set_own_exception)(
        struct ClientProtected *,
        TRCLException *
    );
} ClientProtected;


struct TRCLUserInfo * trcl_client_get_user_info(Client *, TRCLUserId);
struct TRCLListUserInfo * trcl_client_get_subscribed_users_list(Client *);


#endif
