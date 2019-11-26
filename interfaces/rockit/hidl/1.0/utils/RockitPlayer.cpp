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
#define LOG_TAG "RockitPlayer"

#include <dlfcn.h>
#include <android-base/logging.h>
#include "RockitPlayer.h"
#include <hidl/HidlBinderSupport.h>
#include "RTAudioSinkCallback.h"
#include "RTNativeWindowCallback.h"
#include "RTUtils.h"
#include "hwbinder/Parcel.h"

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {
namespace utils {

using namespace ::android;
using namespace ::android::hardware;

#define ROCKIT_PLAYER_LIB_NAME          "/vendor/lib/librockit.so"
#define CREATE_PLAYER_FUNC_NAME         "createRockitPlayer"
#define DESTROY_PLAYER_FUNC_NAME        "destroyRockitPlayer"

#define CREATE_METADATA_FUNC_NAME       "createRockitMetaData"
#define DESTROY_METADATA_FUNC_NAME      "destroyRockitMetaData"

#define RT_TEXT_NOTIFY_MSG              99

enum MediaTrackType {
    MEDIA_TRACK_TYPE_UNKNOWN = 0,
    MEDIA_TRACK_TYPE_VIDEO = 1,
    MEDIA_TRACK_TYPE_AUDIO = 2,
    MEDIA_TRACK_TYPE_TIMEDTEXT = 3,
    MEDIA_TRACK_TYPE_SUBTITLE = 4,
    MEDIA_TRACK_TYPE_METADATA = 5,
};

enum RTTrackType {
    RTTRACK_TYPE_UNKNOWN = -1,  // < Usually treated as AVMEDIA_TYPE_DATA
    RTTRACK_TYPE_VIDEO,
    RTTRACK_TYPE_AUDIO,
    RTTRACK_TYPE_DATA,          // < Opaque data information usually continuous
    RTTRACK_TYPE_SUBTITLE,
    RTTRACK_TYPE_ATTACHMENT,    // < Opaque data information usually sparse

    RTTRACK_TYPE_MEDIA,         // this is not a really type of tracks
                                // it means video,audio,subtitle

    RTTRACK_TYPE_MAX
};


RockitPlayer::RockitPlayer()
              : mPlayerImpl(NULL),
                mPlayerLibFd(NULL),
                mCreatePlayerFunc(NULL),
                mDestroyPlayerFunc(NULL),
                mPlayerCallback(NULL),
                mAudioSinkCallback(NULL),
                mNativeWindowCallback(NULL),
                mRTPlayerCallback(NULL),
                mRTAudioSinkCallback(NULL),
                mRTNativeWindowCallback(NULL){
    ALOGV("RockitPlayer(%p) construct", this);
}

RockitPlayer::~RockitPlayer() {
    ALOGV("~RockitPlayer(%p) destruct", this);
}

Return<Status> RockitPlayer::createPlayer() {
    ALOGV("createPlayer");
    mPlayerLibFd = dlopen(ROCKIT_PLAYER_LIB_NAME, RTLD_LAZY);
    if (mPlayerLibFd == NULL) {
        ALOGE("Cannot load library %s dlerror: %s",
               ROCKIT_PLAYER_LIB_NAME, dlerror());
    }

    mCreatePlayerFunc = (createRockitPlayerFunc *)dlsym(mPlayerLibFd,
                                                        CREATE_PLAYER_FUNC_NAME);
    if(mCreatePlayerFunc == NULL) {
        ALOGE("dlsym for create player failed, dlerror: %s", dlerror());
    }

    mDestroyPlayerFunc = (destroyRockitPlayerFunc *)dlsym(mPlayerLibFd,
                                                        DESTROY_PLAYER_FUNC_NAME);
    if(mDestroyPlayerFunc == NULL) {
        ALOGE("dlsym for destroy player failed, dlerror: %s", dlerror());
    }

    mCreateMetaDataFunc = (createRockitPlayerFunc *)dlsym(mPlayerLibFd,
                                                            CREATE_METADATA_FUNC_NAME);
    if(mCreateMetaDataFunc == NULL) {
        ALOGE("dlsym for create meta data failed, dlerror: %s", dlerror());
    }

    mDestroyMetaDataFunc = (destroyRockitPlayerFunc *)dlsym(mPlayerLibFd,
                                                        DESTROY_METADATA_FUNC_NAME);
    if(mDestroyMetaDataFunc == NULL) {
        ALOGE("dlsym for destroy meta data failed, dlerror: %s", dlerror());
    }

    mPlayerImpl = (RTNDKMediaPlayerInterface *)mCreatePlayerFunc();
    if (mPlayerImpl == NULL) {
        ALOGE("create player failed, player is null");
    }
    ALOGV("player : %p", mPlayerImpl);
    return Status::OK;
}

Return<Status> RockitPlayer::destroyPlayer() {
    mDestroyPlayerFunc((void **)&mPlayerImpl);
    if (mPlayerLibFd != NULL) {
        dlclose(mPlayerLibFd);
        mPlayerLibFd = NULL;
    }

    mPlayerCallback = NULL;
    mAudioSinkCallback = NULL;
    mNativeWindowCallback = NULL;

    if (mRTAudioSinkCallback != NULL) {
        delete mRTAudioSinkCallback;
        mRTAudioSinkCallback = NULL;
    }

    if (mRTNativeWindowCallback != NULL) {
        delete mRTNativeWindowCallback;
        mRTNativeWindowCallback = NULL;
    }

    if (mRTPlayerCallback != NULL) {
        delete mRTPlayerCallback;
        mRTPlayerCallback = NULL;
    }
    ALOGV("RockitPlayer::destroyPlayer %p ok", mPlayerImpl);
    return Status::OK;
}

Return<Status> RockitPlayer::initCheck() {
    ALOGV("initCheck in %p", this);
    return Status::OK;
}

Return<Status> RockitPlayer::setDataSource(
            hidl_vec<uint8_t> const& httpService,
            hidl_string const& url,
            hidl_vec<uint8_t> const& headers) {
    (void)headers;
    hidl_vec<uint8_t> service(httpService);
    //hidl_vec<uint8_t> header(headers);
    ALOGV("setDataSource httpService: %p, url: %s",
           static_cast<void const*>(service.data()),
           url.c_str());
    mPlayerImpl->setDataSource(url.c_str(), NULL);
    return Status::OK;
}

Return<Status> RockitPlayer::setNativeWindow(
            hidl_vec<uint8_t> const& window) {
    hidl_vec<uint8_t> hidl_window(window);
    ALOGV("setNativeWindow window: %p window[0]: %p",
            hidl_window.data(), ((void **)hidl_window.data())[0]);
    mPlayerImpl->setVideoSurface(((void **)hidl_window.data())[0]);
    return Status::OK;
}

Return<Status> RockitPlayer::start() {
    ALOGE("%s %d in", __FUNCTION__, __LINE__);
    mPlayerImpl->start();
    return Status::OK;
}

Return<Status> RockitPlayer::prepare() {
    ALOGV("prepare in");
    mPlayerImpl->prepare();
    return Status::OK;
}

Return<Status> RockitPlayer::prepareAsync() {
    ALOGV("prepareAsync in");
    mPlayerImpl->prepareAsync();
    return Status::OK;
}

Return<Status> RockitPlayer::stop() {
    ALOGV("stop in");
    mPlayerImpl->stop();
    return Status::OK;
}

Return<Status> RockitPlayer::pause() {
    ALOGV("pause in");
    mPlayerImpl->pause();
    return Status::OK;
}

Return<bool>   RockitPlayer::isPlaying() {
    ALOGV("isPlaying in state: %d", mPlayerImpl->getState());
    return (mPlayerImpl->getState() == 1 << 4/*RT_STATE_STARTED*/) ? true : false;
}

Return<Status> RockitPlayer::seekTo(
            int32_t msec,
            uint32_t mode) {
    ALOGD("seekto time: %d, mode: %d", msec, mode);
    mPlayerImpl->seekTo(((int64_t)msec) * 1000);
    return Status::OK;
}

Return<void> RockitPlayer::getCurrentPosition(
            getCurrentPosition_cb _hidl_cb) {
    int64_t usec = 0;
    mPlayerImpl->getCurrentPosition(&usec);
    ALOGV("getCurrentPosition usec: %lld in", (long long)usec);
    _hidl_cb(Status::OK, (int32_t)(usec / 1000));
    return Void();
}

Return<void> RockitPlayer::getDuration(
            getDuration_cb _hidl_cb) {
    int64_t usec = 0;
    mPlayerImpl->getDuration(&usec);
    ALOGV("getDuration usec: %lld in", (long long)usec);
    _hidl_cb(Status::OK, (int32_t)(usec / 1000));
    return Void();
}

Return<Status> RockitPlayer::reset() {
    ALOGV("reset in");
    mPlayerImpl->reset();
    return Status::OK;
}

Return<Status> RockitPlayer::setLooping(int32_t loop) {
    ALOGV("setLooping loop: %d", loop);
    mPlayerImpl->setLooping(loop);
    return Status::OK;
}

Return<player_type> RockitPlayer::playerType() {
    ALOGV("playerType in");
    return (player_type)6;
}

int32_t RockitPlayer::translateMediaType(int32_t rtMediaType) {
    int32_t mediaType = MEDIA_TRACK_TYPE_UNKNOWN;
    switch(rtMediaType) {
        case RTTRACK_TYPE_VIDEO:
            mediaType = MEDIA_TRACK_TYPE_VIDEO;
            break;
        case RTTRACK_TYPE_AUDIO:
            mediaType = MEDIA_TRACK_TYPE_AUDIO;
            break;
        case RTTRACK_TYPE_SUBTITLE:
            mediaType = MEDIA_TRACK_TYPE_TIMEDTEXT;
            break;
        default:
            ALOGD("translateMediaType type = %d not support", rtMediaType);
            break;
    }

    return mediaType;
}

int32_t RockitPlayer::fillTrackInfoReply(RtMetaDataInterface* meta, RockitInvokeReply* reply) {
    int counter = 0;
    void* tracks = NULL;

    RTBool status = meta->findInt32(kUserInvokeTracksCount, &counter);
    if(status == RT_FLASE) {
        ALOGE("fillTrackInfoReply : not find track in meta,counter = %d", counter);
        return -1;
    }
    status = meta->findPointer(kUserInvokeTracksInfor, &tracks);
    if(status == RT_FLASE) {
        ALOGE("fillTrackInfoReply : not find trackInfor in meta");
        return -1;
    }

    reply->event = INVOKE_ID_GET_TRACK_INFO;
    reply->tracks.resize(counter);

    RockitTrackInfor* trackInfor = (RockitTrackInfor*)tracks;
    for(int32_t i = 0; i < counter; i++) {
        reply->tracks[i].codecType = translateMediaType(trackInfor[i].mCodecType);
        reply->tracks[i].codecID   = trackInfor[i].mCodecID;
        reply->tracks[i].width = trackInfor[i].mWidth;
        reply->tracks[i].height = trackInfor[i].mHeight;
        reply->tracks[i].frameRate = trackInfor[i].mFrameRate;
        reply->tracks[i].channelLayout = trackInfor[i].mChannelLayout;
        reply->tracks[i].channels = trackInfor[i].mChannels;
        reply->tracks[i].sampleRate = trackInfor[i].mSampleRate;

        reply->tracks[i].lang = hidl_string(trackInfor[i].lang);
        reply->tracks[i].mine = hidl_string(trackInfor[i].mine);
    }

    return 0;
}

int32_t RockitPlayer::fillInvokeReply(int32_t event, RtMetaDataInterface* meta, RockitInvokeReply* reply) {
    switch (event) {
        case INVOKE_ID_GET_TRACK_INFO: {
            fillTrackInfoReply(meta, reply);
        } break;
    }

    return 0;
}

/*
 * translate command and parameters rockit can use
 */
int32_t RockitPlayer::getInvokeRequest(const ::rockchip::hardware::rockit::V1_0::RockitInvokeEvent& event, RtMetaDataInterface* meta) {
    switch (event.event) {
        case INVOKE_ID_GET_TRACK_INFO: {
            meta->setInt32(kUserInvokeCmd, INVOKE_ID_GET_TRACK_INFO);
        } break;

        case INVOKE_ID_SELECT_TRACK: {
            int32_t trackIdx = event.data[0];
            meta->setInt32(kUserInvokeCmd, INVOKE_ID_SELECT_TRACK);
            meta->setInt32(kUserInvokeTrackIdx, trackIdx);
        } break;
    }

    return 0;
}

Return<void> RockitPlayer::invoke(const ::rockchip::hardware::rockit::V1_0::RockitInvokeEvent& event, IRockitPlayer::invoke_cb _hidl_cb) {
    ALOGV("RockitPlayer::invoke event = %d",event.event);
    rt_status status = OK;
    RockitInvokeReply reply;
    reply.event = event.event;

    RtMetaDataInterface* in = (RtMetaDataInterface *)mCreateMetaDataFunc();
    RtMetaDataInterface* out = (RtMetaDataInterface *)mCreateMetaDataFunc();

    getInvokeRequest(event, in);

    status = mPlayerImpl->invoke(in, out);
    fillInvokeReply(event.event, out, &reply);
    _hidl_cb(Status::OK, reply);

    mDestroyMetaDataFunc((void **)&in);
    mDestroyMetaDataFunc((void **)&out);
    return Void();
}

Return<void>   RockitPlayer::setAudioSink(
            hidl_vec<uint8_t> const& audioSink) {
    hidl_vec<uint8_t> const& hidl_audioSink(audioSink);
    ALOGV("setAudioSink audioSink: %p audioSink[0]: 0x%x",
            hidl_audioSink.data(), ((uint32_t *)hidl_audioSink.data())[0]);
    return Void();
}

Return<Status> RockitPlayer::setParameter(
            int32_t key,
            hidl_vec<uint8_t> const& request) {
    hidl_vec<uint8_t> const& hidl_request(request);
    ALOGV("setParameter key: %d, request: %p", key, hidl_request.data());
    return Status::OK;
}

Return<Status> RockitPlayer::registerPlayerCallback(
        const ::android::sp<::rockchip::hardware::rockit::V1_0::IRTPlayerCallback>& callback) {
    ALOGV("register player callback in");
    mPlayerCallback = callback;
    mRTPlayerCallback = new RTPlayerCallback(this);
    mPlayerImpl->setListener(mRTPlayerCallback);
    return Status::OK;
}

Return<Status> RockitPlayer::registerAudioSinkCallback(
        const ::android::sp<::rockchip::hardware::rockit::V1_0::IRTAudioSinkCallback>& callback) {
    ALOGV("register audiosink callback in");
    mAudioSinkCallback = callback;
    mRTAudioSinkCallback = new RTAudioSinkCallback(this);
    mPlayerImpl->setAudioSink(mRTAudioSinkCallback);
    return Status::OK;
}

Return<Status> RockitPlayer::registerNativeWindowCallback(
            const ::android::sp<::rockchip::hardware::rockit::V1_0::IRTNativeWindowCallback> &callback) {
    mNativeWindowCallback = callback;
    mRTNativeWindowCallback = new RTNativeWindowCallback(this);
    mPlayerImpl->setVideoSurfaceCB(mRTNativeWindowCallback);
    return Status::OK;
}

RTPlayerCallback::RTPlayerCallback(sp<RockitPlayer> player) {
    mPlayer = player;
}

RTPlayerCallback::~RTPlayerCallback() {
    mPlayer.clear();
    mPlayer = NULL;
    ALOGV("~RTPlayerCallback(%p) destruct", this);
}


void RTPlayerCallback::notify(INT32 msg, INT32 ext1, INT32 ext2, void* obj) {
    NotifyTimeTextInfo notifyInfo;
    if (obj != NULL && msg == RT_TEXT_NOTIFY_MSG) {
        const char *text;
        RtMetaDataInterface* textInfo = (RtMetaDataInterface*)obj;
        textInfo->findInt64(kUserNotifyPts, &notifyInfo.startTime);
        textInfo->findInt32(kUserNotifySize, &notifyInfo.size);
        textInfo->findCString(kUserNotifyData, &text);
        notifyInfo.text      = hidl_string(text);
    }
    mPlayer->mPlayerCallback->sendEvent(msg, ext1, ext2, notifyInfo);
}

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware
}  // namespace rockchip

