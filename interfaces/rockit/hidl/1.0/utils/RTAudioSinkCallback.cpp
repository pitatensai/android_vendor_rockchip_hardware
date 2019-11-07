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
#define LOG_TAG "RTAudioSinkCallback"

#include <dlfcn.h>
#include <android-base/logging.h>
#include <hidl/HidlBinderSupport.h>
#include "RTUtils.h"
#include "RockitPlayer.h"
#include "RTAudioSinkCallback.h"

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {
namespace utils {

using namespace ::android;

RTAudioSinkCallback::RTAudioSinkCallback(sp<RockitPlayer> player) {
    mPlayer = player;
}

int RTAudioSinkCallback::open(void *param) {
    ALOGV("RTAudioSinkCallback open in");
    RTAudioSinkParam *audioSinkParam = (RTAudioSinkParam *)param;
    return toStatusT(mPlayer->mCallback->open(audioSinkParam->sampleRate,
                                   audioSinkParam->channels,
                                   audioSinkParam->channelMask,
                                   (int32_t)audioSinkParam->format,
                                   -1, /* buffer count */
                                   NULL, /* cookie */
                                   (int32_t)audioSinkParam->flags,
                                   NULL, /* offload info */
                                   false, /* do not reconnect */
                                   0 /* suggested frame count */));
}

int RTAudioSinkCallback::start() {
    ALOGV("RTAudioSinkCallback start in");
    return toStatusT(mPlayer->mCallback->start());
}

int RTAudioSinkCallback::pause() {
    ALOGV("RTAudioSinkCallback pause in");
    return toStatusT(mPlayer->mCallback->pause());
}

int RTAudioSinkCallback::stop() {
    ALOGV("RTAudioSinkCallback stop in");
    return toStatusT(mPlayer->mCallback->stop());
}

int RTAudioSinkCallback::flush() {
    ALOGV("RTAudioSinkCallback flush in");
    return toStatusT(mPlayer->mCallback->flush());
}

int RTAudioSinkCallback::close() {
    ALOGV("RTAudioSinkCallback close in");
    return toStatusT(mPlayer->mCallback->close());
}

int32_t RTAudioSinkCallback::latency() {
    ALOGV("RTAudioSinkCallback latency in");
    return mPlayer->mCallback->latency();
}

int32_t RTAudioSinkCallback::write(const void *buffer, int32_t size) {
    ALOGV("RTAudioSinkCallback write audio(data=%p, size=%d)", buffer, size);
    int32_t consumedLen = 0;
    hidl_vec<uint8_t> bufParam = inHidlBytes(buffer, size);
    consumedLen = mPlayer->mCallback->write(bufParam, size);
    return consumedLen;
}

int32_t RTAudioSinkCallback::frameSize() {
    ALOGV("RTAudioSinkCallback frameSize in");
    return mPlayer->mCallback->frameSize();
}

int32_t RTAudioSinkCallback::getPlaybackRate(RTAudioPlaybackRate *param) {
    ALOGV("RTAudioSinkCallback start in");
    status_t fnStatus;
    mPlayer->mCallback->getPlaybackRate(
            [&fnStatus, param](
                    Status status, ::rockchip::hardware::rockit::V1_0::AudioPlaybackRate outRate) {
                fnStatus = toStatusT(status);
                param->mSpeed = outRate.mSpeed;
                param->mPitch = outRate.mPitch;
                param->mStretchMode = (RTAudioTimestretchStretchMode)outRate.mStretchMode;
                param->mFallbackMode = (RTAudioTimestretchFallbackMode)outRate.mFallbackMode;

            });
    return fnStatus;
}

int32_t RTAudioSinkCallback::setPlaybackRate(RTAudioPlaybackRate param) {
    ALOGV("RTAudioSinkCallback start in");
    ::rockchip::hardware::rockit::V1_0::AudioPlaybackRate rate;
    rate.mSpeed = param.mSpeed;
    rate.mPitch = param.mPitch;
    rate.mStretchMode = (int32_t)param.mStretchMode;
    rate.mFallbackMode = (int32_t)param.mFallbackMode;
    return toStatusT(mPlayer->mCallback->setPlaybackRate(rate));
}

int64_t RTAudioSinkCallback::getPlayedOutDurationUs() {
    ALOGV("getPlayedOutDurationUs in");
    return mPlayer->mCallback->getPlayedOutDurationUs();
}

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware
}  // namespace rockchip

