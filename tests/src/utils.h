#ifndef TRCL_TEST_UTILS_H
#define TRCL_TEST_UTILS_H


#include "terra-client.h"


struct TRCLClientConfig get_env_config(void);
void log_user_info(struct TRCLUserInfo const *);
void log_list_user_info(struct TRCLListUserInfo const *);


#endif
