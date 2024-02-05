#ifndef TRCL_MODELS_H
#define TRCL_MODELS_H


#include "./exceptions.h"
#include "./config.h"
#include <stddef.h>


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


struct TRCLListUserInfo {
    struct TRCLUserInfo *values;
    size_t count;
};


struct TRCLUserInfo * trcl_model_user_info_alloc(void);
struct TRCLUserInfo * trcl_model_user_info_copy_alloc(
    struct TRCLUserInfo const *
);
void trcl_model_user_info_destroy(struct TRCLUserInfo *);


struct TRCLListUserInfo * trcl_model_list_user_info_alloc(size_t);
void trcl_model_list_user_info_destroy(struct TRCLListUserInfo *);
void trcl_model_user_info_move(
    struct TRCLUserInfo * const,
    struct TRCLUserInfo * const
);
struct TRCLUserInfo * trcl_model_list_user_info_append(
    struct TRCLListUserInfo *,
    struct TRCLUserInfo const *
);
struct TRCLUserInfo * trcl_model_list_user_info_own_append(
    struct TRCLListUserInfo *,
    struct TRCLUserInfo *
);
void trcl_model_list_user_info_own_concat(
    struct TRCLListUserInfo *,
    struct TRCLListUserInfo *
);


#endif
