/*
 * Copyright (C) 2009 The Android Open Source Project
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
#define LOG_TAG "RockitPlayerCallback"
#include <utils/Log.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <string.h>

#include "include/RockitPlayerCallback.h"
#include "include/RockitPlayerInterface.h"

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;

RockitPlayerCallback::RockitPlayerCallback(android::MediaPlayerInterface *player) {
    mPlayer = player;
    ALOGV("RockitPlayerCallback::RockitPlayerCallback");
}

RockitPlayerCallback::~RockitPlayerCallback() {
    ALOGV("RockitPlayerCallback::~RockitPlayerCallback");
}


Return<void> RockitPlayerCallback::sendEvent(int32_t msg, int32_t ext1, int32_t ext2) {
    mPlayer->sendEvent(msg, ext1, ext2);
    return Void();
}

Return<Status> RockitPlayerCallback::open(uint32_t sampleRate,
                    int32_t  channelCount,
                    int32_t  channelMask,
                    int32_t  format,
                    int32_t  bufferCount,
                    /* Bytes callback, TODO */
                    hidl_vec<uint8_t> const& cookie,
                    int32_t  flags,
                    hidl_vec<uint8_t> const& offloadInfo,
                    bool     doNotReconnect,
                    uint32_t suggestedFrameCount) {
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    ALOGV("open in audio sink: %p", player->getAudioSink().get());
    player->getAudioSink()->open(sampleRate,
                                  channelCount,
                                  channelMask,
                                  (audio_format_t)format,
                                  bufferCount != -1 ? bufferCount : DEFAULT_AUDIOSINK_BUFFERCOUNT,
                                  NULL,
                                  (void *)cookie.data(),
                                  (audio_output_flags_t)flags,
                                  (const audio_offload_info_t *)offloadInfo.data(),
                                  doNotReconnect,
                                  suggestedFrameCount);
    return Status::OK;
}

Return<Status> RockitPlayerCallback::start() {
    ALOGV("start in");
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    player->getAudioSink()->start();
    return Status::OK;
}

Return<Status> RockitPlayerCallback::pause() {
    ALOGV("pause in");
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    player->getAudioSink()->pause();
    return Status::OK;
}

Return<Status> RockitPlayerCallback::stop() {
    ALOGV("stop in");
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    player->getAudioSink()->stop();
    return Status::OK;
}

Return<Status> RockitPlayerCallback::flush() {
    ALOGV("flush in");
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    player->getAudioSink()->flush();
    return Status::OK;
}

Return<Status> RockitPlayerCallback::close() {
    ALOGV("close in");
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    player->getAudioSink()->close();
    return Status::OK;
}

Return<int32_t> RockitPlayerCallback::write(
            hidl_vec<uint8_t> const& buffer,
            int32_t size) {
    ALOGV("write(data=%p, size=%d)", buffer.data(), size);
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    return player->getAudioSink()->write(buffer.data(), size);
}

Return<int32_t> RockitPlayerCallback::latency() {
    ALOGV("latency in");
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    return player->getAudioSink()->latency();
}

Return<int32_t> RockitPlayerCallback::frameSize() {
    ALOGV("frameSize in");
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    return player->getAudioSink()->frameSize();
}

Return<void>  RockitPlayerCallback::getPlaybackRate(getPlaybackRate_cb _hidl_cb) {
    ALOGV("getPlaybackRate in");
    AudioPlaybackRate playbackRate;
    android::AudioPlaybackRate playbackRateTmp;
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    player->getAudioSink()->getPlaybackRate(&playbackRateTmp);

    playbackRate.mSpeed = playbackRateTmp.mSpeed;
    playbackRate.mPitch = playbackRateTmp.mPitch;
    playbackRate.mStretchMode = playbackRateTmp.mStretchMode;
    playbackRate.mFallbackMode = playbackRateTmp.mFallbackMode;
    _hidl_cb(Status::OK, playbackRate);
    return Void();
}

Return<Status>  RockitPlayerCallback::setPlaybackRate(const ::rockchip::hardware::rockit::V1_0::AudioPlaybackRate& param) {
    ALOGV("setPlaybackRate in");
    android::AudioPlaybackRate playbackRateTmp;
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    playbackRateTmp.mSpeed = param.mSpeed;
    playbackRateTmp.mPitch = param.mPitch;
    playbackRateTmp.mStretchMode = (android::AudioTimestretchStretchMode)param.mStretchMode;
    playbackRateTmp.mFallbackMode = (android::AudioTimestretchFallbackMode)param.mFallbackMode;
    player->getAudioSink()->setPlaybackRate(playbackRateTmp);
    return Status::OK;
}

Return<int64_t>  RockitPlayerCallback::getPlayedOutDurationUs() {
    int64_t playedUs;
    android::RockitPlayerClient *player = (android::RockitPlayerClient *)mPlayer;
    playedUs = player->getAudioSink()->getPlayedOutDurationUs(systemTime(SYSTEM_TIME_MONOTONIC) / 1000ll);
    ALOGV("getPlayedOutDurationUs playedUs: %lld ", (long long)playedUs);
    return playedUs;
}

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware


