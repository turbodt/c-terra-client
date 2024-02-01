#ifndef TRCL_REQUESTS_SHARED_H
#define TRCL_REQUESTS_SHARED_H


#include "../exceptions.h"


struct TRCLResponse {
    struct TRCLException const * (*get_exception)(struct TRCLResponse const *);
    long int (*get_status_code)(struct TRCLResponse const *);
    void const * (*get_body)(struct TRCLResponse const *);
    void (*destroy)(struct TRCLResponse *);
};


struct TRCLResponseBodyFail {
    char const * message;
};


#endif
