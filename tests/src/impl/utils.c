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


void log_user_info(struct TRCLUserInfo const * user_info) {
    LOG(
        "%s\t%-32s\t%d\t%s\t%s\t%s\t%s\n",
        user_info->user_id,
        user_info->reference_id,
        user_info->is_active,
        user_info->provider,
        user_info->scopes,
        user_info->created_at,
        user_info->last_webhook_update_at
    );
}


void log_list_user_info(struct TRCLListUserInfo const *list) {
    for (size_t i = 0; i < list->count; i++) {
        log_user_info(&list->values[i]);
    }
};
