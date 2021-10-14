#pragma once


#include "open62541.h"

#ifdef __cplusplus
    #define API extern "C"
#else
    #define API extern
#endif


API void registerNodes(UA_Server*);

#undef API