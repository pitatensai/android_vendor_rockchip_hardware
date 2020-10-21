#ifndef _RKNN_BRIDGE_UTILS_
#define _RKNN_BRIDGE_UTILS_

#include <log/log.h>
#define LOG_TAG "RockchipNN"

#define RECORD_TAG() \
ALOGI("+++ %s +++", __func__); \
int ret = 0

#define CheckContext() \
if (context != ctx) { \
    return V1_0::ErrorStatus::RKNN_ERR_FAIL; \
}




#endif // _RKNN_BRIDGE_UTILS_
