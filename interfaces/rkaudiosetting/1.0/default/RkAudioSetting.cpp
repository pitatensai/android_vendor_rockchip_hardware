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

#define LOG_TAG "RkAudioSetting-service"
#include <android-base/logging.h>
#include <log/log.h>
#include <string.h>

#include "RkAudioSetting.h"
#include "RkAudioSettingManager.h"

using namespace android;

namespace rockchip {
namespace hardware {
namespace rkaudiosetting {
namespace V1_0 {
namespace implementation {

RkAudioSetting::RkAudioSetting() {
    mAudioSetting = (RkAudioSettingInterface*) new RkAudioSettingManager();
    mAudioSetting->init();
}

RkAudioSetting::~RkAudioSetting() {
    if (mAudioSetting) {
        delete(mAudioSetting);
        mAudioSetting = NULL;
    }
}

Return<void> RkAudioSetting::setSelect(uint32_t device) {
   mAudioSetting->setSelect(device);
   return Void();
}

Return<void> RkAudioSetting::setFormat(uint32_t device, uint32_t close, const hidl_string &format){
    mAudioSetting->setFormat(device, close, format.c_str());
    return Void();
}

Return<void> RkAudioSetting::setMode(uint32_t device, uint32_t mode){
    mAudioSetting->setMode(device, mode);
    return Void();
}

Return<void> RkAudioSetting::getSelect(uint32_t device, getSelect_cb _hidl_cb){
    int val;
    val = mAudioSetting->getSelect(device);
    _hidl_cb(Status::OK, val);
    return Void();
}

Return<void> RkAudioSetting::getMode(uint32_t device, getMode_cb _hidl_cb){
    int val;
    val = mAudioSetting->getMode(device);
    _hidl_cb(Status::OK, val);
    return Void();
}

Return<void> RkAudioSetting::getFormat(uint32_t device, const hidl_string &format,getFormat_cb _hidl_cb){
    int val;
    val = mAudioSetting->getFormat(device, format.c_str());
    _hidl_cb(Status::OK, val);
    return Void();
}

Return<void> RkAudioSetting::updataFormatForEdid() {
    mAudioSetting->updataFormatForEdid();
    return Void();
}

IRkAudioSetting* HIDL_FETCH_IRkAudioSetting(const char* /* name */) {
    return new RkAudioSetting();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace rkaudiosetting
}  // namespace hardware
}  // namespace rockchip
