/*
 * Copyright 2019 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: rimon.xu@rock-chips.com
 *   date: 2019/06/17
 */

#ifndef SRC_RT_PLAYER_RTNATIVEWINDOWCALLBACK_H_
#define SRC_RT_PLAYER_RTNATIVEWINDOWCALLBACK_H_

#include <sys/types.h>
#include <inttypes.h>
#include "rockit/hidl/1.0/RTNativeWindowCBInterface.h"

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {
namespace utils {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::IBinder;
using ::android::sp;
using ::android::wp;

struct RockitPlayer;

class RTNativeWindowCallback : public RTNativeWindowCBInterface {
 public:
    RTNativeWindowCallback(sp<RockitPlayer> player);
    virtual ~RTNativeWindowCallback();

    virtual int setCrop(void *nativeWindow, int32_t left, int32_t top, int32_t right, int32_t bottom);

    virtual int setBufferCount(void *nativeWindow, int32_t bufferCount);

    virtual int connect(void *nativeWindow, int32_t mode);

    virtual int disconnect(void *nativeWindow, int32_t mode);

    virtual int setScalingMode(void *nativeWindow, int32_t mode);

    virtual int setUsage(void *nativeWindow, int32_t usage);

    virtual int query(void *nativeWindow, int32_t cmd, int32_t *param);

    virtual int setBufferGeometry(void *nativeWindow, int32_t width, int32_t height, int32_t format);

    virtual int dequeueBufferAndWait(void *nativeWindow, RTNativeWindowBufferInfo *info);

    virtual int queueBuffer(void *nativeWindow, void *buf, int32_t fence);

    virtual int dequeueBuffer(void *nativeWindow, void **buf);

    virtual int cancelBuffer(void *nativeWindow, void *buf, int32_t fence);

    virtual int setSwapInterval(void *nativeWindow, int32_t isInterval);

    virtual int setDataSpace(void *nativeWindow, int32_t dataSpace);

 private:
    sp<RockitPlayer> mPlayer;
};

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware
}  // namespace rockchip

#endif  // SRC_RT_PLAYER_RTNATIVEWINDOWCALLBACK_H_

