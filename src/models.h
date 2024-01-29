#ifndef TRCL_MODELS_H
#define TRCL_MODELS_H


#include "./exceptions.h"
#include "./config.h"


typedef char const * TRCLReferenceId;
typedef char const * TRCLUserId;
typedef char const * TRCLUserScopes;
typedef char const * TRCLTimestamp;
typedef char const * TRCLProvider;


struct TRCLUserInfo {
    TRCLUserId user_id;
    TRCLReferenceId reference_id;
    TRCL_BOOL is_active;
    TRCLTimestamp created_at;
    TRCLTimestamp last_webhook_update_at;
    TRCLProvider provider;
    TRCLUserScopes scopes;
};


struct TRCLUserInfo * trcl_model_user_info_alloc(void);
void trcl_model_user_info_destroy(struct TRCLUserInfo *);


#endif
