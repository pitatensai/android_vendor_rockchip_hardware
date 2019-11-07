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

#ifndef ROCKIT_HIDL_V1_0_UTILS_ROCKITPLAYER_H
#define ROCKIT_HIDL_V1_0_UTILS_ROCKITPLAYER_H

#include <rockchip/hardware/rockit/1.0/IRockitPlayer.h>
#include <rockchip/hardware/rockit/1.0/IRockitPlayerCallback.h>
#include <rockchip/hardware/rockit/1.0/IRTNativeWindowCallback.h>

#include <hidl/Status.h>
#include <hwbinder/IBinder.h>
#include <hwbinder/Parcel.h>
#include "rockit/hidl/1.0/RTNDKMediaPlayerInterface.h"
#include "rockit/hidl/1.0/RTAudioSinkCBInterface.h"
#include "rockit/hidl/1.0/RTNativeWindowCBInterface.h"
#include "rockit/hidl/1.0/RTMedaDataInterface.h"

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
using ::rockchip::hardware::rockit::V1_0::player_type;
using ::android::sp;
using ::android::wp;

typedef void * createRockitPlayerFunc();
typedef void   destroyRockitPlayerFunc(void **player);

typedef void * createRockitMetaDataFunc();
typedef void   destroyRockitMetaDataFunc(void **meta);


/* must keep sync with mediaplayer.h and rockit */
enum InvokeIds {
    INVOKE_ID_GET_TRACK_INFO = 1,
    INVOKE_ID_ADD_EXTERNAL_SOURCE = 2,
    INVOKE_ID_ADD_EXTERNAL_SOURCE_FD = 3,
    INVOKE_ID_SELECT_TRACK = 4,
    INVOKE_ID_UNSELECT_TRACK = 5,
    INVOKE_ID_SET_VIDEO_SCALING_MODE = 6,
    INVOKE_ID_GET_SELECTED_TRACK = 7
};

/* must keep sync with RTTrackInfor in rockit */
typedef struct _RockitTrackInfo {
    int32_t  mCodecType;
    int32_t  mCodecID;
    uint32_t mCodecOriginID;
    int32_t  mIdx;

    /* video track features */
    int32_t  mWidth;
    int32_t  mHeight;
    float    mFrameRate;

    /* audio track features*/
    int64_t  mChannelLayout;
    int32_t  mChannels;
    int32_t  mSampleRate;

    /* subtitle track features*/


    /* language */
    char lang[4];
    char mine[16];
} RockitTrackInfor;

struct RockitPlayer : public IRockitPlayer {
    RockitPlayer();

    Return<Status> createPlayer();
    Return<Status> destroyPlayer();

    virtual Return<Status> initCheck();
    virtual Return<Status> setDataSource(
                hidl_vec<uint8_t> const& httpService,
                hidl_string const& url,
                hidl_vec<uint8_t> const& headers);
    virtual Return<Status> setNativeWindow(
                hidl_vec<uint8_t> const& window);
    virtual Return<Status> prepare();
    virtual Return<Status> prepareAsync();
    virtual Return<Status> start();
    virtual Return<Status> stop();
    virtual Return<Status> pause();
    virtual Return<bool>   isPlaying();
    virtual Return<Status> seekTo(
                int32_t msec,
                uint32_t mode);
    virtual Return<void> getCurrentPosition(
                getCurrentPosition_cb _hidl_cb);
    virtual Return<void> getDuration(
                getDuration_cb _hidl_cb);
    virtual Return<Status> reset();
    virtual Return<Status> setLooping(int32_t loop);
    virtual Return<player_type> playerType();
    virtual Return<void> invoke(const ::rockchip::hardware::rockit::V1_0::RockitInvokeEvent& event,
            IRockitPlayer::invoke_cb _hidl_cb);
    virtual Return<void>  setAudioSink(
                hidl_vec<uint8_t> const& audioSink);
    virtual Return<Status> setParameter(
                int32_t key,
                hidl_vec<uint8_t> const& request);

    virtual Return<Status> registerCallback(
                const ::android::sp<::rockchip::hardware::rockit::V1_0::IRockitPlayerCallback>& callback);

    virtual Return<Status> registerNativeWindowCallback(
                const ::android::sp<::rockchip::hardware::rockit::V1_0::IRTNativeWindowCallback> &callback);

protected:
    virtual ~RockitPlayer();

    virtual int32_t getInvokeRequest(const ::rockchip::hardware::rockit::V1_0::RockitInvokeEvent& event, RtMetaDataInterface* meta);
    virtual int32_t fillTrackInfoReply(RtMetaDataInterface* meta, RockitInvokeReply* reply) ;
    virtual int32_t fillInvokeReply(int32_t event, RtMetaDataInterface* meta, RockitInvokeReply* reply);
    virtual int32_t translateMediaType(int32_t rtMediaType);

    RTNDKMediaPlayerInterface   *mPlayerImpl;

    // rockit player lib impl
    void                        *mPlayerLibFd;
    createRockitPlayerFunc      *mCreatePlayerFunc;
    destroyRockitPlayerFunc     *mDestroyPlayerFunc;

    createRockitMetaDataFunc    *mCreateMetaDataFunc;
    destroyRockitMetaDataFunc   *mDestroyMetaDataFunc;

public:
    sp<IRockitPlayerCallback>    mCallback;
    sp<IRTNativeWindowCallback>  mNativeWindowCallback;
};

class RockitMsgCallback : public RTPlayerListener {
public:
    RockitMsgCallback(sp<RockitPlayer> player);
    virtual void notify(INT32 msg, INT32 ext1, INT32 ext2, void* ptr);
private:
    sp<RockitPlayer> mPlayer;
};

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware
}  // namespace rockchip

#endif  // ROCKIT_HIDL_V1_0_UTILS_ROCKITPLAYER_H
