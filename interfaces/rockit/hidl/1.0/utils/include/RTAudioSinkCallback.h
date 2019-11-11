/*
 * Copyright 2018 The Android Open Source Project
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
 */

#ifndef ROCKIT_HIDL_V1_0_UTILS_AUDIOSINKCALLBACK_H
#define ROCKIT_HIDL_V1_0_UTILS_AUDIOSINKCALLBACK_H

#include <rockchip/hardware/rockit/1.0/IRockitPlayerCallback.h>

#include <hidl/Status.h>
#include <hwbinder/IBinder.h>
#include "rockit/hidl/1.0/RTAudioSinkCBInterface.h"
#include "RockitPlayer.h"

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

class RTAudioSinkCallback : public RTAudioSinkCBInterface {
public:
    RTAudioSinkCallback(sp<RockitPlayer> player);
    virtual int open(void *param);
    virtual int start();
    virtual int pause();
    virtual int stop();
    virtual int flush();
    virtual int close();

    virtual int32_t write(const void *buffer, int32_t size);

    virtual uint32_t latency();
    virtual int32_t frameSize();
    virtual int32_t getPlaybackRate(RTAudioPlaybackRate *param);
    virtual int32_t setPlaybackRate(RTAudioPlaybackRate param);
    virtual int64_t getPlayedOutDurationUs();

private:
    sp<RockitPlayer> mPlayer;
};

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware
}  // namespace rockchip

#endif  // ROCKIT_HIDL_V1_0_UTILS_AUDIOSINKCALLBACK_H
