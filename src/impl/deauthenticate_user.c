#include "../protected/client.h"
#include "../requests/deauthenticate-user.h"
#include "../models.h"
#include "../base.h"


void trcl_client_deauthenticate_user(Client *client, TRCLUserId user_id) {
    ClientProtected * pclient = (ClientProtected *) client;

    struct TRCLResponseDeauthenticateUser * response;
    response = trcl_request_deauthenticate_user(&pclient->config, user_id);

    pclient->set_own_exception(pclient, response->exception);
    response->exception = NULL;

    trcl_response_deauthenticate_user_destroy(response);
};
