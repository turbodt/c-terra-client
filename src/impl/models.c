#include "../models.h"
#include "../shared/string.h"
#include "../base.h"


static void trcl_model_user_info_init(struct TRCLUserInfo *);
static void trcl_model_user_info_clear(struct TRCLUserInfo *);
void trcl_model_user_info_copy(
    struct TRCLUserInfo * const,
    struct TRCLUserInfo const * const
);
static void trcl_model_list_user_info_resize(
    struct TRCLListUserInfo *,
    size_t
);


struct TRCLUserInfo * trcl_model_user_info_alloc(void) {
    struct TRCLUserInfo * model = TRCL_ALLOC(struct TRCLUserInfo, 1);
    TRCL_ASSERT_ALLOC(model);
    trcl_model_user_info_init(model);
    return model;
};

struct TRCLUserInfo * trcl_model_user_info_copy_alloc(
    struct TRCLUserInfo const *src
) {
    struct TRCLUserInfo * model = trcl_model_user_info_alloc();
    trcl_model_user_info_copy(model, src);
    return model;
};

void trcl_model_user_info_destroy(struct TRCLUserInfo *model) {
    RETURN_IF_NULL(model);
    trcl_model_user_info_clear(model);
    TRCL_FREE(model);
};


struct TRCLListUserInfo * trcl_model_list_user_info_alloc(size_t count) {
    struct TRCLListUserInfo * model = TRCL_ALLOC(struct TRCLListUserInfo, 1);
    TRCL_ASSERT_ALLOC(model);

    model->count = 0;
    model->values = NULL;

    trcl_model_list_user_info_resize(model, count);

    return model;
};


void trcl_model_list_user_info_destroy(struct TRCLListUserInfo *model) {
    RETURN_IF_NULL(model);
    trcl_model_list_user_info_resize(model, 0);
    TRCL_FREE(model);
};


struct TRCLUserInfo * trcl_model_list_user_info_append(
    struct TRCLListUserInfo * list,
    struct TRCLUserInfo const * model
) {
    struct TRCLUserInfo * new_model = trcl_model_user_info_copy_alloc(model);
    return trcl_model_list_user_info_own_append(list, new_model);
};


struct TRCLUserInfo * trcl_model_list_user_info_own_append(
    struct TRCLListUserInfo * list,
    struct TRCLUserInfo * model
) {
    trcl_model_list_user_info_resize(list, list->count + 1);
    struct TRCLUserInfo * new_model = &list->values[list->count -1];

    trcl_model_user_info_move(new_model, model);
    trcl_model_user_info_destroy(model);

    return new_model;
};


void trcl_model_list_user_info_own_concat(
    struct TRCLListUserInfo * dst,
    struct TRCLListUserInfo * src
) {
    RETURN_IF_NULL(src);
    size_t offset = dst->count;
    trcl_model_list_user_info_resize(dst, dst->count + src->count);
    for(size_t i = 0; i < src->count; i++) {
        trcl_model_user_info_move(&dst->values[offset + i], &src->values[i]);
    }

    trcl_model_list_user_info_destroy(src);
};


void trcl_model_user_info_move(
    struct TRCLUserInfo * const dst,
    struct TRCLUserInfo * const src
) {
    trcl_model_user_info_clear(dst);
    dst->is_active = src->is_active;
    dst->user_id = src->user_id;
    dst->reference_id = src->reference_id;
    dst->provider = src->provider;
    dst->scopes = src->scopes;
    dst->created_at = src->created_at;
    dst->last_webhook_update_at = src->last_webhook_update_at;
    trcl_model_user_info_init(src);
};


void trcl_model_user_info_init(struct TRCLUserInfo *model) {
    RETURN_IF_NULL(model);
    *model = (struct TRCLUserInfo){
        .is_active = FALSE,
        .user_id = NULL,
        .reference_id = NULL,
        .provider = NULL,
        .scopes = NULL,
        .created_at = NULL,
        .last_webhook_update_at = NULL,
    };
};

void trcl_model_user_info_clear(struct TRCLUserInfo *model) {
    RETURN_IF_NULL(model);
    str_destroy(model->user_id);
    str_destroy(model->reference_id);
    str_destroy(model->provider);
    str_destroy(model->scopes);
    str_destroy(model->created_at);
    str_destroy(model->last_webhook_update_at);
    trcl_model_user_info_init(model);
};


void trcl_model_user_info_copy(
    struct TRCLUserInfo * const dst,
    struct TRCLUserInfo const * const src
) {
    trcl_model_user_info_clear(dst);
    dst->is_active = src->is_active;
    dst->user_id = str_copy_alloc(src->user_id);
    dst->reference_id = str_copy_alloc(src->reference_id);
    dst->provider = str_copy_alloc(src->provider);
    dst->scopes = str_copy_alloc(src->scopes);
    dst->created_at = str_copy_alloc(src->created_at);
    dst->last_webhook_update_at = str_copy_alloc(src->last_webhook_update_at);
};


void trcl_model_list_user_info_resize(
    struct TRCLListUserInfo * model,
    size_t new_count
) {
    size_t count = model->count;

    for (size_t i = new_count ; i < count; i++) {
        trcl_model_user_info_clear(&model->values[i]);
    }

    if (new_count == 0) {
        TRCL_FREE(model->values);
        model->values = NULL;
    } else {
        model->values = TRCL_REALLOC(
            model->values,
            struct TRCLUserInfo,
            new_count
        );
        TRCL_ASSERT_ALLOC(model->values);
    }

    for (size_t i = count; i < new_count; i++) {
        trcl_model_user_info_init(&model->values[i]);
    }

    model->count = new_count;
};
