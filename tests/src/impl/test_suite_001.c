#include "../test_suite_001.h"
#include "../base.h"
#include "../utils.h"
#include "terra-cli.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>


#define USER_ID "abd2582b-ba7f-48b0-9e95-a776f5023dbe"


TEST_H(test_001);
TEST_H(test_002);
TEST_H(test_003);


static void (*tests[])(void) = {
    TEST_NAME(test_001),
    TEST_NAME(test_002),
    TEST_NAME(test_003),
    NULL
};


void (*test_suite_001_get_test(size_t index))(void) {
    return tests[index];
};


TEST(test_001, {

    struct TRCLClientConfig const config = get_env_config();

    struct TRCLException * exception = NULL;
    struct TRCLClient * client = trcl_client_alloc(&config, &exception);
    ASSERT_FALSE(trcl_exception_get_code(exception));

    struct TRCLUserInfo * user_info = client->get_user_info(client, USER_ID);

    if (client->get_last_exception_code(client)) {
        LOG(
            "%s\n",
            trcl_exception_get_message(client->get_last_exception(client))
        );
    }
    ASSERT_FALSE(client->get_last_exception_code(client));

    LOG("\n\n");
    log_user_info(user_info);

    trcl_model_user_info_destroy(user_info);
    client->destroy(client);
});


TEST(test_002, {

    struct TRCLClientConfig const config = get_env_config();

    struct TRCLException * exception = NULL;
    struct TRCLClient * client = trcl_client_alloc(&config, &exception);
    ASSERT_FALSE(trcl_exception_get_code(exception));

    struct TRCLListUserInfo * user_list = client->get_subscribed_users_list(
        client
    );

    if (client->get_last_exception_code(client)) {
        LOG(
            "%s\n",
            trcl_exception_get_message(client->get_last_exception(client))
        );
    }
    ASSERT_FALSE(client->get_last_exception_code(client));

    LOG("\n\n");
    log_list_user_info(user_list);

    trcl_model_list_user_info_destroy(user_list);
    client->destroy(client);
});


TEST(test_003, {
    struct TRCLClientConfig const config = get_env_config();

    struct TRCLException * exception = NULL;
    struct TRCLClient * client = trcl_client_alloc(&config, &exception);
    ASSERT_FALSE(trcl_exception_get_code(exception));

    client->deauthenticate_user(client, USER_ID);

    if (client->get_last_exception_code(client)) {
        LOG(
            "%s\n",
            trcl_exception_get_message(client->get_last_exception(client))
        );
    }
    ASSERT_FALSE(client->get_last_exception_code(client));

    client->destroy(client);
});
