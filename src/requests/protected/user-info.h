#ifndef TRCL_REQUESTS_PROTECTED_USER_INFO_H
#define TRCL_REQUESTS_PROTECTED_USER_INFO_H


#include "../shared.h"
#include <jansson.h>


struct TRCLUserInfo * user_info_from_json_alloc(
    json_t const *,
    struct TRCLException **
);


#endif
