#include "../protected/client.h"
#include "../requests/user-info.h"
#include "../models.h"
#include "../base.h"
#include <curl/curl.h>
#include <string.h>


static void trcl_client_destroy(Client *);
static TRCLException const * trcl_client_get_last_exception(Client const *);
static TRCLExceptionCode trcl_client_get_last_exception_code(Client const *);
static ClientProtected * trcl_client_set_own_exception(
    ClientProtected *,
    TRCLException *
);


Client * trcl_client_alloc(ClientConfig const * config) {
    ClientProtected * client = TRCL_ALLOC(ClientProtected, 1);
    TRCL_ASSERT_ALLOC(client);

    client->config = *config;
    client->last_exception = trcl_exception_ok_alloc();
    client->set_own_exception = trcl_client_set_own_exception;
    client->base = (Client) {
        .destroy = trcl_client_destroy,
        .get_last_exception = trcl_client_get_last_exception,
        .get_last_exception_code = trcl_client_get_last_exception_code,
        .get_user_info = trcl_client_get_user_info,
    };

    return (Client *) client;
};


void trcl_client_destroy(Client *client) {
    ClientProtected * pclient = (ClientProtected *) client;
    trcl_exception_destroy(pclient->last_exception);
    TRCL_FREE(pclient);
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
