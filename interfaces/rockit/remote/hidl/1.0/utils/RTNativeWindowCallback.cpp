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

//#define LOG_NDEBUG 0
#define LOG_TAG "RTNativeWindowCallback"

#include <string.h>
#include <android-base/logging.h>
#include <hidl/HidlBinderSupport.h>
#include "RTNativeWindowCallback.h"
#include "RockitPlayer.h"
#include "RTUtils.h"

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {
namespace utils {

using namespace ::android;

RTNativeWindowCallback::RTNativeWindowCallback(sp<RockitPlayer> player) {
    mPlayer = player;
}

RTNativeWindowCallback::~RTNativeWindowCallback() {
    mPlayer.clear();
    mPlayer = NULL;
    ALOGV("~RTNativeWindowCallback(%p) destruct", this);
}

int RTNativeWindowCallback::setCrop(
        void *nativeWindow,
        int32_t left,
        int32_t top,
        int32_t right,
        int32_t bottom) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->setCrop(
                              windowParam, left, top, right, bottom));
}

int RTNativeWindowCallback::setBufferCount(void *nativeWindow, int32_t bufferCount) {
    ALOGE("%s %d in", __FUNCTION__, __LINE__);

    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->setBufferCount(
                              windowParam, bufferCount));
}

int RTNativeWindowCallback::connect(void *nativeWindow, int32_t mode) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    ALOGE("%s %d in", __FUNCTION__, __LINE__);
    return toStatusT(mPlayer->mNativeWindowCallback->apiConnect(
                              windowParam, mode));
}

int RTNativeWindowCallback::disconnect(void *nativeWindow, int32_t mode) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    ALOGE("%s %d in", __FUNCTION__, __LINE__);
    return toStatusT(mPlayer->mNativeWindowCallback->apiDisconnect(
                              windowParam, mode));
}

int RTNativeWindowCallback::setScalingMode(void *nativeWindow, int32_t mode) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    ALOGE("%s %d in", __FUNCTION__, __LINE__);
    return toStatusT(mPlayer->mNativeWindowCallback->setScalingMode(
                              windowParam, mode));
}

int RTNativeWindowCallback::setUsage(void *nativeWindow, int32_t usage) {
    ALOGE("%s %d in", __FUNCTION__, __LINE__);
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->setUsage(
                              windowParam, usage));
}

int RTNativeWindowCallback::query(void *nativeWindow, int32_t cmd, int32_t *param) {
    int fnStatus;
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    mPlayer->mNativeWindowCallback->query(
            windowParam,
            cmd,
            [&fnStatus, param](
                    Status status, uint32_t outParam) {
                fnStatus = toStatusT(status);
                *param = outParam;
            });
    return fnStatus;
}

int RTNativeWindowCallback::setBufferGeometry(
        void *nativeWindow,
        int32_t width,
        int32_t height,
        int32_t format) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    ALOGE("%s %d in", __FUNCTION__, __LINE__);
    return toStatusT(mPlayer->mNativeWindowCallback->setBuffersGeometry(
                              windowParam, width, height, format));
}

int RTNativeWindowCallback::dequeueBufferAndWait(void *nativeWindow, RTNativeWindowBufferInfo *info) {
    int fnStatus;
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    mPlayer->mNativeWindowCallback->dequeueBufferAndWait(
             windowParam,
             [&fnStatus, info](
                   Status status, hidl_vec<uint8_t> outInfo) {
               fnStatus = toStatusT(status);
               memcpy(info, outInfo.data(), sizeof(RTNativeWindowBufferInfo));
             });

    return fnStatus;
}

int RTNativeWindowCallback::queueBuffer(void *nativeWindow, void *buf, int32_t fence) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    hidl_vec<uint8_t> bufParam = inHidlBytes((void *)&buf, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->queueBuffer(
                              windowParam, bufParam, fence));
}

int RTNativeWindowCallback::dequeueBuffer(void *nativeWindow, void **buf) {
    int fnStatus;
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    mPlayer->mNativeWindowCallback->dequeueBufferAndWait(
             windowParam,
             [&fnStatus, buf](
                   Status status, hidl_vec<uint8_t> outbuf) {
               fnStatus = toStatusT(status);
               memcpy(buf, outbuf.data(), sizeof(void *));
             });
    return fnStatus;
}

int RTNativeWindowCallback::cancelBuffer(void *nativeWindow, void *buf, int32_t fence) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    hidl_vec<uint8_t> bufParam = inHidlBytes((void *)&buf, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->cancelBuffer(
                              windowParam, bufParam, fence));
}

int RTNativeWindowCallback::setSwapInterval(void *nativeWindow, int32_t isInterval) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->setSwapInterval(
                              windowParam, isInterval));
}

int RTNativeWindowCallback::setDataSpace(void *nativeWindow, int32_t dataSpace) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->setDataSpace(
                              windowParam, dataSpace));
}

int RTNativeWindowCallback::setTransform(void *nativeWindow, int32_t transform) {
    hidl_vec<uint8_t> windowParam = inHidlBytes((void *)&nativeWindow, sizeof(void *));
    return toStatusT(mPlayer->mNativeWindowCallback->setTransform(
                              windowParam, transform));
}


}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware
}  // namespace rockchip

