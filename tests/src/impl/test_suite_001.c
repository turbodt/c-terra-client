#include "../test_suite_001.h"
#include "../base.h"
#include "terra-cli.h"
#include <string.h>
#include <curl/curl.h>


static struct TRCLClientConfig const config = {
    .api_key = "",
    .dev_id = "",
    .base_url = "https://api.tryterra.co/v2/",
};


TEST_H(test_001);


static void (*tests[])(void) = {
    TEST_NAME(test_001),
    NULL
};


void (*test_suite_001_get_test(size_t index))(void) {
    return tests[index];
};

TEST(test_001, {
    curl_global_init(CURL_GLOBAL_ALL);
    struct TRCLClient * client = trcl_client_alloc(&config);

    struct TRCLUserInfo * user_info = client->get_user_info(
        client,
        "aacab2e7-610f-4714-b309-3b4ff2cf04dd"
    );

    ASSERT_FALSE(client->get_last_exception_code(client));

    LOG("\n\n");
    LOG(
        "%s\t%-15s\t%d\t%s\t%s\t%s\t%s\n",
        user_info->user_id,
        user_info->reference_id,
        user_info->is_active,
        user_info->provider,
        user_info->scopes,
        user_info->created_at,
        user_info->last_webhook_update_at
    );

    trcl_model_user_info_destroy(user_info);
    client->destroy(client);
    curl_global_cleanup();
});
