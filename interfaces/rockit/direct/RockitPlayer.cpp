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

#include <stdint.h>
#include <dlfcn.h>

#include "RockitPlayer.h"
#include "media/mediaplayer.h"

namespace android {

#define ROCKIT_PLAYER_LIB_NAME          "/system/lib/librockit.so"
#define CREATE_PLAYER_FUNC_NAME         "createRockitPlayer"
#define DESTROY_PLAYER_FUNC_NAME        "destroyRockitPlayer"
#define CREATE_METADATA_FUNC_NAME       "createRockitMetaData"
#define DESTROY_METADATA_FUNC_NAME      "destroyRockitMetaData"

/* must keep sync with rockit */
typedef enum  _RTInvokeIds {
    RT_INVOKE_ID_GET_TRACK_INFO = 1,
    RT_INVOKE_ID_ADD_EXTERNAL_SOURCE = 2,
    RT_INVOKE_ID_ADD_EXTERNAL_SOURCE_FD = 3,
    RT_INVOKE_ID_SELECT_TRACK = 4,
    RT_INVOKE_ID_UNSELECT_TRACK = 5,
    RT_INVOKE_ID_SET_VIDEO_SCALING_MODE = 6,
    RT_INVOKE_ID_GET_SELECTED_TRACK = 7
} RTInvokeIds;

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
    char     lang[16];
    char     mine[16];

    bool     mProbeDisabled;
    /* use reserved first when extend this structure */
    int8_t   mReserved[64];
} RockitTrackInfor;

RockitPlayer::RockitPlayer()
              : mPlayerImpl(NULL),
                mPlayerLibFd(NULL),
                mCreatePlayerFunc(NULL),
                mDestroyPlayerFunc(NULL),
                mCreateMetaDataFunc(NULL),
                mDestroyMetaDataFunc(NULL) {
    ALOGD("RockitPlayer(%p) construct", this);
}

RockitPlayer::~RockitPlayer() {
    ALOGD("~RockitPlayer(%p) destruct", this);
}

status_t RockitPlayer::createPlayer() {
    ALOGV("createPlayer");

    mPlayerLibFd = dlopen(ROCKIT_PLAYER_LIB_NAME, RTLD_LAZY);
    if (mPlayerLibFd == NULL) {
        ALOGE("Cannot load library %s dlerror: %s", ROCKIT_PLAYER_LIB_NAME, dlerror());
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
    return OK;
}

status_t RockitPlayer::destroyPlayer() {
    ALOGV("destroyPlayer");
    mDestroyPlayerFunc((void **)&mPlayerImpl);
    mPlayerImpl = NULL;
    dlclose(mPlayerLibFd);
    return OK;
}

status_t RockitPlayer::initCheck() {
    ALOGV("initCheck in");
    return OK;
}

status_t RockitPlayer::setDataSource(
            void *httpService,
            const char *url,
            void *headers) {
    (void)headers;
    (void)httpService;

    ALOGV("setDataSource url: %s", url);
    mPlayerImpl->setDataSource(url, NULL);
    return OK;
}

rt_status RockitPlayer::setDataSource(
            int fd,
            int64_t offset,
            int64_t length) {
    ALOGV("setDataSource url: fd = %d", fd);
    return mPlayerImpl->setDataSource(fd, offset, length);
}

rt_status RockitPlayer::setNativeWindow(const void *window) {
    ALOGV("setNativeWindow window: %p", window);
    return mPlayerImpl->setVideoSurface(const_cast<void *>(window));
}

rt_status RockitPlayer::start() {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    mPlayerImpl->start();
    return OK;
}

rt_status RockitPlayer::prepare() {
    ALOGV("prepare in");
    mPlayerImpl->prepare();
    return OK;
}

rt_status RockitPlayer::prepareAsync() {
    ALOGV("prepareAsync in");
    mPlayerImpl->prepareAsync();
    return OK;
}

rt_status RockitPlayer::stop() {
    ALOGV("stop in");
    mPlayerImpl->stop();
    return OK;
}

rt_status RockitPlayer::pause() {
    ALOGV("pause in");
    mPlayerImpl->pause();
    return OK;
}

bool RockitPlayer::isPlaying() {
    ALOGV("isPlaying in state: %d", mPlayerImpl->getState());
    return (mPlayerImpl->getState() == 1 << 4/*RT_STATE_STARTED*/) ? true : false;
}

rt_status RockitPlayer::seekTo(int32_t msec, uint32_t mode) {
    ALOGD("seekTo time: %d, mode: %d", msec, mode);
    mPlayerImpl->seekTo(((int64_t)msec) * 1000);
    return OK;
}

rt_status RockitPlayer::getCurrentPosition(int *msec) {
    int64_t usec = 0;
    mPlayerImpl->getCurrentPosition(&usec);
    ALOGV("getCurrentPosition usec: %lld in", (long long)usec);
    *msec = (int32_t)(usec / 1000);
    return OK;
}

rt_status RockitPlayer::getDuration(int *msec) {
    int64_t usec = 0;
    mPlayerImpl->getDuration(&usec);
    ALOGV("getDuration usec: %lld in", (long long)usec);
    *msec = (int32_t)(usec / 1000);
    return OK;
}

rt_status RockitPlayer::reset() {
    ALOGV("reset in");
    mPlayerImpl->reset();
    return OK;
}

rt_status RockitPlayer::setLooping(int32_t loop) {
    ALOGV("setLooping loop: %d", loop);
    mPlayerImpl->setLooping(loop);
    return OK;
}

int32_t RockitPlayer::playerType() {
    ALOGV("playerType in");
    return 6;
}

int32_t RockitPlayer::fillInvokeRequest(const Parcel &request, RtMetaData* meta, int32_t& event) {
    int32_t methodId;
    status_t ret = request.readInt32(&methodId);
    if (ret != OK) {
        return ret;
    }

    event = methodId;
    switch (methodId) {
        case INVOKE_ID_GET_TRACK_INFO: {
            meta->setInt32(kUserInvokeCmd, RT_INVOKE_ID_GET_TRACK_INFO);
        } break;

        case INVOKE_ID_SELECT_TRACK:
        case INVOKE_ID_UNSELECT_TRACK:    {
            int index = request.readInt32();
            int cmd = (methodId == INVOKE_ID_SELECT_TRACK)?
                    RT_INVOKE_ID_SELECT_TRACK:RT_INVOKE_ID_UNSELECT_TRACK;
            meta->setInt32(kUserInvokeCmd, cmd);
            meta->setInt32(kUserInvokeTrackIdx, index);
        } break;

        case INVOKE_ID_SET_VIDEO_SCALING_MODE: {
            int mode = request.readInt32();
            meta->setInt32(kUserInvokeCmd, RT_INVOKE_ID_SET_VIDEO_SCALING_MODE);
            meta->setInt32(kUserInvokeVideoScallingMode, mode);
        } break;

        case INVOKE_ID_GET_SELECTED_TRACK: {
            meta->setInt32(kUserInvokeCmd, RT_INVOKE_ID_GET_SELECTED_TRACK);
        } break;

        default:
            ALOGD("RockitPlayer::fillInvokeRequest: methodid = %d not supprot, add codes here", methodId);
            ret = BAD_VALUE;
            break;
    }

    return ret;
}

rt_status RockitPlayer::translateMediaType(int32_t rtMediaType) {
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

void RockitPlayer::fillTrackInfor(Parcel *reply, int type,String16& mime, String16& lang) {
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

rt_status RockitPlayer::fillTrackInfoReply(RtMetaData* meta, Parcel* reply) {
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

    reply->writeInt32(counter);

    char desc[100];
    String16 mine, lang;
    RockitTrackInfor* trackInfor = (RockitTrackInfor*)tracks;
    for (int32_t i = 0; i < counter; ++i) {
        int codecType = translateMediaType(trackInfor[i].mCodecType);
        switch (codecType) {
            case MEDIA_TRACK_TYPE_VIDEO: {
                    snprintf(desc, 100, ",%dx%d,%f", trackInfor[i].mWidth,
                             trackInfor[i].mHeight, trackInfor[i].mFrameRate);
                    String16 mine = String16(trackInfor[i].mine);
                    String16 lang = String16(desc) + mine;
                    fillTrackInfor(reply, MEDIA_TRACK_TYPE_VIDEO, mine, lang);
                } break;

            case MEDIA_TRACK_TYPE_AUDIO: {
                snprintf(desc, 100, ",%d,%d,", trackInfor[i].mSampleRate,
                          trackInfor[i].mChannels);
                String16 mine = String16(trackInfor[i].mine);
                String16 lang = mine + String16(desc);
                fillTrackInfor(reply, (int)MEDIA_TRACK_TYPE_AUDIO, mine, lang);
            } break;

            case MEDIA_TRACK_TYPE_SUBTITLE: {
                mine = String16(trackInfor[i].mine);
                lang = String16(trackInfor[i].lang);
                fillTrackInfor(reply, (int)MEDIA_TRACK_TYPE_SUBTITLE, mine, lang);
            } break;

            case MEDIA_TRACK_TYPE_TIMEDTEXT: {
                mine = String16(trackInfor[i].mine);
                lang = String16(trackInfor[i].lang);
                fillTrackInfor(reply, (int)MEDIA_TRACK_TYPE_TIMEDTEXT, mine, lang);
            } break;
        }
    }

    return 0;
}

rt_status RockitPlayer::fillGetSelectedTrackReply(RtMetaData* meta, Parcel* reply) {
    int idx = 0;
    RTBool status = meta->findInt32(kUserInvokeGetSelectTrackIdx, &idx);
    if(status == RT_FLASE) {
        ALOGE("fillTrackInfoReply : not find track in meta,counter = %d", idx);
        idx = -1;
    }

    reply->writeInt32(idx);
    return OK;
}

rt_status RockitPlayer::fillInvokeReply(int32_t event, RtMetaData* meta, Parcel* reply) {
    rt_status ret = OK;
    switch (event) {
        case INVOKE_ID_GET_TRACK_INFO: {
            ret = fillTrackInfoReply(meta, reply);
        } break;

        case INVOKE_ID_GET_SELECTED_TRACK: {
            ret = fillGetSelectedTrackReply(meta, reply);
        } break;
    }

    return ret;
}

rt_status RockitPlayer::invoke(const Parcel &request, Parcel *reply) {
    ALOGD("RockitPlayer::invoke");
    if (reply == NULL) {
        ALOGD("RockitPlayer::invoke, reply == NULL");
        return OK;
    }

    RtMetaData* in = (RtMetaData *)mCreateMetaDataFunc();
    RtMetaData* out = (RtMetaData *)mCreateMetaDataFunc();
    int32_t event = -1;
    // tranlate cmd to rockit can understand
    fillInvokeRequest(request, in, event);

    mPlayerImpl->invoke(in, out);

    // tranlate result to mediaplayer can understand
    fillInvokeReply(event, out, reply);

    mDestroyMetaDataFunc((void **)&in);
    mDestroyMetaDataFunc((void **)&out);

    return OK;
}

void RockitPlayer::setAudioSink(const void *audioSink) {
    ALOGV("setAudioSink audioSink: %p", audioSink);
    mPlayerImpl->setAudioSink(audioSink);
}
void RockitPlayer::setSubteSink(const void *subteSink) {
    ALOGV("setSubteSink subteSink: %p", subteSink);
    mPlayerImpl->setSubteSink(subteSink);
}

rt_status RockitPlayer::setParameter(int key, const Parcel &request) {
    (void)request;
    ALOGV("setParameter key: %d", key);
    return OK;
}

rt_status RockitPlayer::setNativeWindowCallback(void *callback) {
    return mPlayerImpl->setVideoSurfaceCB(callback);
}

rt_status RockitPlayer::setListener(RTPlayerListener *listener) {
    return mPlayerImpl->setListener(listener);
}

rt_status RockitPlayer::setPlaybackSettings(const AudioPlaybackRate& rate) {
    RtMetaData* meta = (RtMetaData *)mCreateMetaDataFunc();

    meta->setInt32(kUserInvokeCmd, RT_INVOKE_SET_PLAY_SPEED);
    meta->setFloat(kUserInvokeSetPlaybackRate, rate.mSpeed);

    rt_status status = mPlayerImpl->invoke(meta, NULL);

    mDestroyMetaDataFunc((void **)&meta);
    return status;
}

}

