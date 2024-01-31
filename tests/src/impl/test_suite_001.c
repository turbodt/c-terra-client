#include "../test_suite_001.h"
#include "../base.h"
#include "../utils.h"
#include "terra-cli.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>


#define USER_ID "21b6ac15-2581-4476-a316-b24533c96094"


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
    curl_global_init(CURL_GLOBAL_ALL);

    struct TRCLClientConfig const config = get_env_config();

    struct TRCLClient * client = trcl_client_alloc(&config);

    struct TRCLUserInfo * user_info = client->get_user_info(client, USER_ID);

    ASSERT_FALSE(client->get_last_exception_code(client));

    LOG("\n\n");
    log_user_info(user_info);

    trcl_model_user_info_destroy(user_info);
    client->destroy(client);
    curl_global_cleanup();
});


TEST(test_002, {
    curl_global_init(CURL_GLOBAL_ALL);

    struct TRCLClientConfig const config = get_env_config();

    struct TRCLClient * client = trcl_client_alloc(&config);

    struct TRCLListUserInfo * user_list = client->get_subscribed_users_list(
        client
    );

    if (client->get_last_exception_code(client)) {
        LOG(
            "\tError:\n\t%s\n",
            trcl_exception_get_message(client->get_last_exception(client))
        );
    }
    ASSERT_FALSE(client->get_last_exception_code(client));

    LOG("\n\n");
    log_list_user_info(user_list);

    trcl_model_list_user_info_destroy(user_list);
    client->destroy(client);
    curl_global_cleanup();
});


TEST(test_003, {
    curl_global_init(CURL_GLOBAL_ALL);

    struct TRCLClientConfig const config = get_env_config();

    struct TRCLClient * client = trcl_client_alloc(&config);

    client->deauthenticate_user(client, USER_ID);

    if (client->get_last_exception_code(client)) {
        LOG(
            "\tError:\n\t%s\n",
            trcl_exception_get_message(client->get_last_exception(client))
        );
    }
    ASSERT_FALSE(client->get_last_exception_code(client));

    client->destroy(client);
    curl_global_cleanup();
});
