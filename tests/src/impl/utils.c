#include "../base.h"
#include "terra-cli.h"
#include <stdlib.h>


struct TRCLClientConfig get_env_config(void) {
    struct TRCLClientConfig config = {
        .api_key = getenv("TERRA_API_KEY"),
        .dev_id = getenv("TERRA_DEV_ID"),
        .base_url = getenv("TERRA_BASE_URL")
    };
    ASSERT_NOT_NULL(config.api_key);
    ASSERT_NOT_NULL(config.dev_id);
    ASSERT_NOT_NULL(config.base_url);
    return config;
};
