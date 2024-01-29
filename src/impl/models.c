#include "../models.h"
#include "../shared/string.h"
#include "../base.h"


struct TRCLUserInfo * trcl_model_user_info_alloc(void) {
    struct TRCLUserInfo * user_info = TRCL_ALLOC(struct TRCLUserInfo, 1);
    TRCL_ASSERT_ALLOC(user_info);

    *user_info = (struct TRCLUserInfo){
        .is_active = FALSE,
        .user_id = NULL,
        .reference_id = NULL,
        .provider = NULL,
        .scopes = NULL,
        .created_at = NULL,
        .last_webhook_update_at = NULL,
    };

    return user_info;
};


void trcl_model_user_info_destroy(struct TRCLUserInfo *user_info) {
    RETURN_IF_NULL(user_info);
    str_destroy(user_info->user_id);
    str_destroy(user_info->reference_id);
    str_destroy(user_info->provider);
    str_destroy(user_info->scopes);
    str_destroy(user_info->created_at);
    str_destroy(user_info->last_webhook_update_at);
    TRCL_FREE(user_info);
};
