/*
 * Copyright (C) 2016 The Android Open Source Project
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
#ifndef RKAUDIOSETTING_V1_0_DEFAULT_RKAUDIOSETTING_H
#define RKAUDIOSETTING_V1_0_DEFAULT_RKAUDIOSETTING_H

#include <rockchip/hardware/rkaudiosetting/1.0/IRkAudioSetting.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "RkAudioSettingInterface.h"

namespace rockchip {
namespace hardware {
namespace rkaudiosetting {
namespace V1_0 {
namespace implementation {

using namespace android;
using namespace rockchip::hardware::rkaudiosetting::V1_0;

using ::rockchip::hardware::rkaudiosetting::V1_0::IRkAudioSetting;
using ::rockchip::hardware::rkaudiosetting::V1_0::Status;

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct RkAudioSetting : public IRkAudioSetting {
    RkAudioSetting();
    virtual ~RkAudioSetting();
    //  set parameter
    Return<void> setSelect(uint32_t device);
    Return<void> setFormat(uint32_t device, uint32_t close, const hidl_string &format);
    Return<void> setMode(uint32_t device, uint32_t mode);

    //  set parameter
    Return<void> getSelect(uint32_t device, getSelect_cb _hidl_cb);
    Return<void> getMode(uint32_t device, getMode_cb _hidl_cb);
    Return<void> getFormat(uint32_t device, const hidl_string &format, getFormat_cb _hidl_cb);
    Return<void> updataFormatForEdid();
 protected:
    RkAudioSettingInterface  *mAudioSetting;
};

extern "C" IRkAudioSetting* HIDL_FETCH_IRkAudioSetting(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace rkaudiosetting
}  // namespace hardware
}  // namespace rockchip

#endif  // RKAUDIOSETTING_V1_0_DEFAULT_RKAUDIOSETTING_H