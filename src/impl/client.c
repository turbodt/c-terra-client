#include "../protected/client.h"
#include "../requests/user-info.h"
#include "../models.h"
#include "../base.h"
#include <curl/curl.h>
#include <string.h>


#define ERR_MSG_CLIENT_ALLOC "Unable to allocate client."
#define ERR_MSG_CURL_NOT_INIT "Unable to initialize curl."


static void trcl_client_destroy(Client *);
static TRCLException const * trcl_client_get_last_exception(Client const *);
static TRCLExceptionCode trcl_client_get_last_exception_code(Client const *);
static ClientProtected * trcl_client_set_own_exception(
    ClientProtected *,
    TRCLException *
);
static ClientProtected * trcl_client_set_exception(
    ClientProtected *,
    TRCLException const *
);


Client * trcl_client_alloc(
    ClientConfig const * config,
    struct TRCLException **exception
) {
    ClientProtected * client = TRCL_ALLOC(ClientProtected, 1);

    TRCL_ASSERT_ALLOC_OR(client, {
        *exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNABLE_TO_ALLOC,
            ERR_MSG_CLIENT_ALLOC
        );
        return NULL;
    });

    client->config = *config;
    client->last_exception = NULL;
    client->set_own_exception = trcl_client_set_own_exception;
    client->set_exception = trcl_client_set_exception;
    client->base = (Client) {
        .destroy = trcl_client_destroy,
        .get_last_exception = trcl_client_get_last_exception,
        .get_last_exception_code = trcl_client_get_last_exception_code,
        .get_user_info = trcl_client_get_user_info,
        .get_subscribed_users_list = trcl_client_get_subscribed_users_list,
        .deauthenticate_user = trcl_client_deauthenticate_user,
    };

    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        TRCL_FREE(client);
        client = NULL;
        *exception = trcl_exception_alloc(
            TRCL_EXCEPTION__UNABLE_TO_ALLOC,
            ERR_MSG_CURL_NOT_INIT
        );
    } else {
        *exception = trcl_exception_ok_alloc();
    }

    client->last_exception = *exception;

    return (Client *) client;
};


void trcl_client_destroy(Client *client) {
    ClientProtected * pclient = (ClientProtected *) client;
    trcl_exception_destroy(pclient->last_exception);
    TRCL_FREE(pclient);
    curl_global_cleanup();
};


TRCLException const * trcl_client_get_last_exception(Client const *client) {
    ClientProtected * pclient = (ClientProtected *) client;
    return pclient->last_exception;
};


TRCLExceptionCode trcl_client_get_last_exception_code(Client const *client) {
    TRCLException const * exception = client->get_last_exception(client);
    return trcl_exception_get_code(exception);
};


struct ClientProtected * trcl_client_set_own_exception(
    ClientProtected * client,
    TRCLException *e
) {
    trcl_exception_destroy(client->last_exception);
    client->last_exception = e;
    return client;
};


struct ClientProtected * trcl_client_set_exception(
    ClientProtected * client,
    TRCLException const *e
) {
    trcl_exception_destroy(client->last_exception);
    client->last_exception = trcl_exception_copy_alloc(e);
    return client;
};
