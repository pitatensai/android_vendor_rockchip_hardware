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
#define LOG_TAG "RockitPlayerManager"

#include <utils/Log.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <media/Metadata.h>
#include <media/MediaHTTPService.h>
#include <media/mediaplayer.h>
#include <gui/Surface.h>

#include "include/RockitPlayerManager.h"
#include "include/RockitPlayerCallback.h"
#include "include/AudioSinkCallback.h"
#include "include/NativeWindowCallback.h"

#include <rockchip/hardware/rockit/1.0/IRockitPlayer.h>
#include <rockchip/hardware/rockit/1.0/IRTPlayerCallback.h>
#include <rockchip/hardware/rockit/1.0/IRockitPlayerService.h>
#include <rockchip/hardware/rockit/1.0/IRTAudioSinkCallback.h>
#include <rockchip/hardware/rockit/1.0/IRTNativeWindowCallback.h>

namespace android {

using namespace ::rockchip::hardware::rockit::V1_0;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

typedef struct RockitPlayerCtx_t {
     sp<IRockitPlayer>              mPlayer;
     sp<RockitPlayerCallback>       mPlayerCallback;
     sp<RTAudioSink>                mAudioSinkCallback;
     sp<RTNativeWindow>             mNativeWindowCallback;
     sp<MediaPlayerBase::AudioSink> mAudioSink;
     sp<ANativeWindow>              mNativeWindow;
} RockitPlayerCtx;

static status_t toStatusT(Status status) {
    switch (status) {
    case Status::OK:
        return OK;
    default:
        return UNKNOWN_ERROR;
    }
}

inline hidl_vec<uint8_t> inHidlBytes(void const* l, size_t size) {
    hidl_vec<uint8_t> t;
    t.setToExternal(static_cast<uint8_t*>(const_cast<void*>(l)), size, false);
    return t;
}

RockitPlayerManager::RockitPlayerManager(android::MediaPlayerInterface* mediaPlayer) {
    mCtx = (RockitPlayerCtx *)malloc(sizeof(RockitPlayerCtx));
    memset(mCtx, 0, sizeof(RockitPlayerCtx));
    initPlayer(mediaPlayer);
    ALOGV("RockitPlayerManager(%p) construct", this);
}

RockitPlayerManager::~RockitPlayerManager() {
    reset();
    deinitPlayer();
    if (mCtx) {
        free(mCtx);
        mCtx = NULL;
    }
    ALOGV("~RockitPlayerManager(%p) destruct", this);
}

void RockitPlayerManager::initPlayer(android::MediaPlayerInterface* mediaPlayer) {
    sp<IRockitPlayerService> playerService = IRockitPlayerService::getService();
    if (playerService.get() != NULL) {
        status_t fnStatus;
        playerService->createPlayer(
                [&fnStatus, this](
                        Status status, const sp<IRockitPlayer>& player) {
                    fnStatus = toStatusT(status);
                    mCtx->mPlayer = player;
                });
        mCtx->mPlayerCallback = new RockitPlayerCallback(mediaPlayer);
        mCtx->mAudioSinkCallback = new RTAudioSink(mediaPlayer);
        mCtx->mNativeWindowCallback = new RTNativeWindow();
        status_t err = toStatusT(mCtx->mPlayer->registerPlayerCallback(mCtx->mPlayerCallback));
        err = toStatusT(mCtx->mPlayer->registerAudioSinkCallback(mCtx->mAudioSinkCallback));
        err = toStatusT(mCtx->mPlayer->registerNativeWindowCallback(mCtx->mNativeWindowCallback));
        ALOGV("create rockit player(%p) completed(err: %d)", mCtx->mPlayer.get(), err);
    } else {
        ALOGE("failed to get rockit service");
    }
}

void RockitPlayerManager::deinitPlayer() {
    sp<IRockitPlayerService> playerService = IRockitPlayerService::getService();
    if (mCtx->mPlayer.get()) {
        playerService->destroyPlayer(mCtx->mPlayer);
    }

    mCtx->mPlayerCallback = NULL;
    mCtx->mNativeWindowCallback = NULL;
    mCtx->mAudioSinkCallback = NULL;
    mCtx->mPlayer = NULL;
    mCtx->mAudioSink = NULL;
    mCtx->mNativeWindow = NULL;

    ALOGV("deinitPlayer(%p) ok", mCtx->mPlayer.get());
}

status_t RockitPlayerManager::initCheck() {
    ALOGV("initCheck thiz(%p) player(%p)", this, mCtx->mPlayer.get());
    return toStatusT(mCtx->mPlayer->initCheck());
}

status_t RockitPlayerManager::setUID(uid_t uid) {
    (void)uid;
    return OK;
}

status_t RockitPlayerManager::setDataSource(
        const sp<IMediaHTTPService> &httpService,
        const char *url,
        const KeyedVector<String8, String8> *headers) {
    (void)httpService;
    void *params = (void *)headers;
    hidl_vec<uint8_t> tParams = inHidlBytes(params, sizeof(*headers));
    return toStatusT(mCtx->mPlayer->setDataSource(NULL, url, tParams));
}

// Warning: The filedescriptor passed into this method will only be valid until
// the method returns, if you want to keep it, dup it!
status_t RockitPlayerManager::setDataSource(int fd, int64_t offset, int64_t length) {
    ALOGV("setDataSource(%d, %lld, %lld)", fd, (long long)offset, (long long)length);
    char uri[1024] = {0};
    ALOGE("setDataSource: %p", &uri);
    char *params = uri;
    getUriFromFd(fd, &params);
    return toStatusT(mCtx->mPlayer->setDataSource(NULL, uri, NULL));
}

status_t RockitPlayerManager::setDataSource(const sp<IStreamSource> &source) {
    (void)source;
    return OK;
}

status_t RockitPlayerManager::setVideoSurfaceTexture(
        const sp<IGraphicBufferProducer> &bufferProducer) {
    ANativeWindow *window = NULL;
    if (bufferProducer.get() != NULL) {
        window = new Surface(bufferProducer);
    }
    void *params = (void *)window;
    ALOGV("setVideoSurfaceTexture window: %p bufferProducer: %p", params, bufferProducer.get());
    hidl_vec<uint8_t> tParams = inHidlBytes(&params, sizeof(void *));
    mCtx->mNativeWindow = window;
    mCtx->mNativeWindowCallback->setNativeWindow(window);
    return toStatusT(mCtx->mPlayer->setNativeWindow(tParams));
}

status_t RockitPlayerManager::prepare() {
    ALOGV("prepare");
    return toStatusT(mCtx->mPlayer->prepare());
}

status_t RockitPlayerManager::prepareAsync() {
    ALOGV("prepareAsync");
    return toStatusT(mCtx->mPlayer->prepareAsync());
}

status_t RockitPlayerManager::start() {
    ALOGV("start");
    return toStatusT(mCtx->mPlayer->start());
}

status_t RockitPlayerManager::stop() {
    ALOGV("stop");
    return toStatusT(mCtx->mPlayer->stop());
}

status_t RockitPlayerManager::pause() {
    ALOGV("pause");
    return toStatusT(mCtx->mPlayer->pause());
}

bool RockitPlayerManager::isPlaying() {
    ALOGV("isPlaying");
    return mCtx->mPlayer->isPlaying();
}

status_t RockitPlayerManager::seekTo(int msec, MediaPlayerSeekMode mode) {
    ALOGV("seekTo %.2f secs", msec / 1E3);
    return toStatusT(mCtx->mPlayer->seekTo(msec, static_cast<uint32_t>(mode)));
}

status_t RockitPlayerManager::getCurrentPosition(int *msec) {
    ALOGV("getCurrentPosition");
    status_t fnStatus;
    mCtx->mPlayer->getCurrentPosition(
            [&fnStatus, msec](
                    Status status, uint32_t outMsec) {
                fnStatus = toStatusT(status);
                *msec = outMsec;
            });
    return  fnStatus;
}

status_t RockitPlayerManager::getDuration(int *msec) {
    ALOGV("getDuration");
    status_t fnStatus;
    mCtx->mPlayer->getDuration(
            [&fnStatus, msec](
                    Status status, uint32_t outMsec) {
                fnStatus = toStatusT(status);
                *msec = outMsec;
            });
    return fnStatus;
}

status_t RockitPlayerManager::reset() {
    ALOGV("reset");
    return toStatusT(mCtx->mPlayer->reset());
}

status_t RockitPlayerManager::setLooping(int loop) {
    ALOGV("setLooping");
    return toStatusT(mCtx->mPlayer->setLooping(loop));
}

player_type RockitPlayerManager::playerType() {
    ALOGV("playerType");
    return ROCKIT_PLAYER;
}

void RockitPlayerManager::fillTrackInfor(Parcel *reply, int type,String16& mime, String16& lang) {
    if(reply == NULL){
        return;
    }
  //  ALOGV("type = %d, mine = %s, lang = %s", type, mime.string(), lang.string());
    reply->writeInt32(3);
    /* track type */
    reply->writeInt32(type);
    /* mine */
    reply->writeString16(mime);
    /* language */
    reply->writeString16(lang);

    if(type == (int)MEDIA_TRACK_TYPE_SUBTITLE){
        reply->writeInt32(0); // KEY_IS_AUTOSELECT
        reply->writeInt32(0); // KEY_IS_DEFAULT
        reply->writeInt32(0); // KEY_IS_FORCED_SUBTITLE
    }
}


int32_t RockitPlayerManager::fillTrackInfoReply(RockitInvokeReply& meta, Parcel* reply) {
    int32_t count = meta.tracks.size();
    char desc[100];
    String16 mine, lang;

    reply->writeInt32(count);
    for (int32_t i = 0; i < count; ++i) {
        switch (meta.tracks[i].codecType) {
            case MEDIA_TRACK_TYPE_VIDEO: {
                    snprintf(desc, 100, ",%dx%d,%f", meta.tracks[i].width,
                             meta.tracks[i].height, meta.tracks[i].frameRate);
                    String16 mine = String16(meta.tracks[i].mine.c_str());
                    String16 lang = String16(desc) + mine;
                    fillTrackInfor(reply,MEDIA_TRACK_TYPE_VIDEO, mine, lang);
                } break;

            case MEDIA_TRACK_TYPE_AUDIO: {
                snprintf(desc, 100, ",%d,%d,", meta.tracks[i].sampleRate,
                         meta.tracks[i].channels);
                String16 mine = String16(meta.tracks[i].mine.c_str());
                String16 lang = mine + String16(desc);
                fillTrackInfor(reply, (int)MEDIA_TRACK_TYPE_AUDIO, mine, lang);
            } break;

            case MEDIA_TRACK_TYPE_SUBTITLE: {
                mine = String16(meta.tracks[i].mine.c_str());
                lang = String16(meta.tracks[i].lang.c_str());
                fillTrackInfor(reply, (int)MEDIA_TRACK_TYPE_SUBTITLE, mine, lang);
            } break;

            case MEDIA_TRACK_TYPE_TIMEDTEXT: {
                mine = String16(meta.tracks[i].mine.c_str());
                lang = String16(meta.tracks[i].lang.c_str());
                fillTrackInfor(reply, (int)MEDIA_TRACK_TYPE_TIMEDTEXT, mine, lang);
            } break;
        }
    }

    return 0;
}

int32_t RockitPlayerManager::fillInvokeReply(RockitInvokeReply& meta, Parcel* reply) {
    switch (meta.event) {
        case INVOKE_ID_GET_TRACK_INFO: {
                fillTrackInfoReply(meta, reply);
        } break;
    }

    return 0;
}

int32_t RockitPlayerManager::fillInvokeRequest(const Parcel &parcel, RockitInvokeEvent& request) {
    int32_t methodId;
    status_t ret = parcel.readInt32(&methodId);
    if (ret != OK) {
        return ret;
    }

    switch (methodId) {
        case INVOKE_ID_GET_TRACK_INFO: {
            /* cmd */
            request.event = INVOKE_ID_GET_TRACK_INFO;
        } break;

        case INVOKE_ID_SELECT_TRACK: {
            /* cmd */
            request.event = INVOKE_ID_SELECT_TRACK;
            /* parameters */
            int index = parcel.readInt32();
            request.data.resize(1);
            request.data[0] = index;
        } break;

        default:
            ALOGD("RockitPlayerClient::fillInvokeRequest: methodid = %d not supprot, add codes here",methodId);
            ret = BAD_VALUE;
            break;
    }

    return ret;
}

status_t RockitPlayerManager::invoke(const Parcel &request, Parcel *reply) {
    RockitInvokeEvent invokeEvent;
    RockitInvokeReply invokeReply;

    /* translate Paracel to RockitInvokeEvent */
    status_t ret = fillInvokeRequest(request, invokeEvent);
    if (ret != OK) {
        return ret;
    }

    mCtx->mPlayer->invoke(invokeEvent,
                [&ret, &invokeReply](
                    Status status, RockitInvokeReply re) {
                ret = toStatusT(status);
                invokeReply = re;
            });

    /* translate RockitInvokeReply to Parcel */
    fillInvokeReply(invokeReply, reply);
    return OK;//ret;
}

void RockitPlayerManager::setAudioSink(const sp<MediaPlayerBase::AudioSink> &audioSink) {
    ALOGV("setAudioSink audiosink: %p", audioSink.get());
    mCtx->mAudioSink = audioSink;
}

status_t RockitPlayerManager::setParameter(int key, const Parcel &request) {
    ALOGV("setParameter(key=%d)", key);
    void *params = (void *)&request;
    hidl_vec<uint8_t> tParams = inHidlBytes(params, sizeof(Parcel));
    return toStatusT(mCtx->mPlayer->setParameter(key, tParams));
}

status_t RockitPlayerManager::getParameter(int key, Parcel *reply) {
    ALOGV("getParameter");
    (void)key;
    (void)reply;
    return OK;//reinterpret_cast<status_t>(mPlayer->getParameter(key, reply));
}

status_t RockitPlayerManager::getMetadata(
        const media::Metadata::Filter& ids, Parcel *records) {
    ALOGV("getMetadata");
    (void)ids;
    (void)records;
    return OK;//reinterpret_cast<status_t>(mPlayer->getMetadata(ids, records));
}

status_t RockitPlayerManager::getPlaybackSettings(AudioPlaybackRate* rate) {
    (void)rate;
    return OK;//reinterpret_cast<status_t>(mPlayer->getPlaybackSettings(rate));
}

status_t RockitPlayerManager::setPlaybackSettings(const AudioPlaybackRate& rate) {
    (void)rate;
    return OK;//reinterpret_cast<status_t>(mPlayer->setPlaybackSettings(rate));
}

status_t RockitPlayerManager::dump(int fd, const Vector<String16> &args) const {
    (void)args;
    (void)fd;
    return OK;//reinterpret_cast<status_t>(mPlayer->dump(args));
}

sp<MediaPlayerBase::AudioSink> RockitPlayerManager::getAudioSink() {
    return mCtx->mAudioSink;
}


status_t RockitPlayerManager::getUriFromFd(int fd, char **uri) {
    size_t      uriSize = 0;
    char        uriTmp[1024] = {0};
    const char *ptr = NULL;
    String8     path;

    path.appendFormat("/proc/%d/fd/%d", getpid(), fd);
    if ((uriSize = readlink(path.string(), uriTmp, sizeof(uriTmp) - 1)) < 0) {
        return BAD_VALUE;
    } else {
        uriTmp[uriSize] = '\0';
    }

    path = uriTmp;
    ptr  = path.string();
    ALOGE("getUriFromFd ptr: %p, uriSize: %zu, uri: %s *uri: %p uri: %p",
           ptr, uriSize, ptr, *uri, uri);
    memcpy(*uri, ptr, uriSize);

    return OK;
}

}  // namespace android


