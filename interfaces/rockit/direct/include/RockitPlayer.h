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

#ifndef ROCKIT_DIRECT_ROCKITPLAYER_H
#define ROCKIT_DIRECT_ROCKITPLAYER_H

#include <media/MediaPlayerInterface.h>
#include "RTNDKMediaPlayerInterface.h"
#include "RTMedaDataInterface.h"
#include <binder/Parcel.h>

namespace android {

typedef void * createRockitPlayerFunc();
typedef void   destroyRockitPlayerFunc(void **player);

typedef void * createRockitMetaDataFunc();
typedef void   destroyRockitMetaDataFunc(void **meta);

enum RTInvokeId {
    RT_INVOKE_SET_PLAY_SPEED = 10000,
    RT_INVOKE_GET_PLAY_SPEED,
};

class RockitPlayer : public RefBase {
 public:
    RockitPlayer();
    virtual ~RockitPlayer();

    rt_status createPlayer();
    rt_status destroyPlayer();

    virtual rt_status initCheck();
    virtual rt_status setDataSource(
                void *httpService,
                const char *url,
                void *headers);
    virtual rt_status setDataSource(
                int fd,
                int64_t offset,
                int64_t length);
    virtual rt_status setNativeWindow(
                const void *window);
    virtual rt_status prepare();
    virtual rt_status prepareAsync();
    virtual rt_status start();
    virtual rt_status stop();
    virtual rt_status pause();
    virtual bool      isPlaying();
    virtual rt_status seekTo(int32_t msec, uint32_t mode);
    virtual rt_status getCurrentPosition(int *msec);
    virtual rt_status getDuration(int *msec);
    virtual rt_status reset();
    virtual rt_status setLooping(int32_t loop);
    virtual int32_t   playerType();
    virtual rt_status invoke(const Parcel &request, Parcel *reply);
    virtual void      setAudioSink(const void *audioSink);
    virtual void      setSubteSink(const void *subteSink);
    virtual rt_status setParameter(int key, const Parcel &request);

    virtual rt_status setNativeWindowCallback(void *callback);
    virtual rt_status setListener(RTPlayerListener *listener);
    virtual rt_status setPlaybackSettings(const AudioPlaybackRate& rate);

 protected:
    rt_status fillInvokeRequest(const Parcel &parcel, RtMetaDataInterface* meta, int32_t& event);
    rt_status fillInvokeReply(int32_t event, RtMetaDataInterface* meta, Parcel* reply);
    rt_status fillTrackInfoReply(RtMetaDataInterface* meta, Parcel* reply);
    void      fillTrackInfor(Parcel *reply, int type, String16& mime, String16& lang);
    rt_status translateMediaType(int32_t rtMediaType);
    rt_status fillGetSelectedTrackReply(RtMetaDataInterface* meta, Parcel* reply);

 private:
    RTNDKMediaPlayerInterface   *mPlayerImpl;

    // rockit player lib impl
    void                        *mPlayerLibFd;
    createRockitPlayerFunc      *mCreatePlayerFunc;
    destroyRockitPlayerFunc     *mDestroyPlayerFunc;

    createRockitMetaDataFunc    *mCreateMetaDataFunc;
    destroyRockitMetaDataFunc   *mDestroyMetaDataFunc;
};

}

#endif  // ROCKIT_DIRECT_ROCKITPLAYER_H
