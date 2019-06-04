/*
**
** Copyright 2009, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef _ROCKITPLAYERCALLBACK_H_
#define _ROCKITPLAYERCALLBACK_H_

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <media/MediaPlayerInterface.h>
#include <system/audio-base.h>

#include <rockchip/hardware/rockit/1.0/IRockitPlayerCallback.h>

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

class RockitPlayerCallback : public IRockitPlayerCallback {
public:
    RockitPlayerCallback(android::MediaPlayerInterface *player);
    virtual ~RockitPlayerCallback();

    virtual Return<void> sendEvent(int32_t msg, int32_t ext1, int32_t ext2);

    virtual Return<Status> open(uint32_t sampleRate,
                        int32_t  channelCount,
                        int32_t  channelMask,
                        int32_t  format,
                        int32_t  bufferCount,
                        /* Bytes callback, TODO */
                        hidl_vec<uint8_t> const& cookie,
                        int32_t  flags,
                        hidl_vec<uint8_t> const& offloadInfo,
                        bool     doNotReconnect,
                        uint32_t suggestedFrameCount);

    virtual Return<Status> start();
    virtual Return<Status> pause();
    virtual Return<Status> stop();
    virtual Return<Status> flush();
    virtual Return<Status> close();

    virtual Return<int32_t> write(hidl_vec<uint8_t> const& buffer, int32_t size);
    virtual Return<int32_t> latency();
    virtual Return<int32_t> frameSize();
    virtual Return<void>    getPlaybackRate(getPlaybackRate_cb _hidl_cb);
    virtual Return<Status>  setPlaybackRate(const ::rockchip::hardware::rockit::V1_0::AudioPlaybackRate& param);
    virtual Return<int64_t> getPlayedOutDurationUs();

private:
    android::MediaPlayerInterface *mPlayer;
};

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware


#endif  // _ROCKITPLAYERCALLBACK_H_
