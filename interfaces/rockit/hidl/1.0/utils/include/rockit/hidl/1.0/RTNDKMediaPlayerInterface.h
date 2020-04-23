/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 *
 * author: martin.cheng@rock-chips.com
 *   date: 2019/06/12
 * module: RTNDKMediaPlayerInterface
 */

#ifndef SRC_RT_PLAYER_RTNDKMEDIAPLAYERINTERFACE_H_
#define SRC_RT_PLAYER_RTNDKMEDIAPLAYERINTERFACE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <inttypes.h>
#include "rockit/hidl/1.0/RTMetaData.h"

typedef int32_t rt_status;

#ifndef uid_t
#define uid_t int32_t
#endif

#ifndef INT32
#define INT32 int32_t
#endif

class  RTParcel;
class RTPlayerListener {
 public:
    virtual void notify(INT32 msg, INT32 ext1, INT32 ext2, void* ptr) = 0;
    virtual ~RTPlayerListener() {}
};

class RTNDKMediaPlayerInterface {
 public:
    RTNDKMediaPlayerInterface() {}
    virtual ~RTNDKMediaPlayerInterface() {}

    /*
     * basic control operations
     */
    virtual rt_status setUID(uid_t uid) = 0;
    virtual rt_status setDataSource(const char *url, const char *headers) = 0;
    virtual rt_status setDataSource(int fd, int64_t offset, int64_t length) = 0;
    virtual rt_status setLooping(int loop) = 0;
    virtual rt_status setListener(RTPlayerListener* listener) = 0;

    /*
     * basic operations (dipatch controls by messages)
     */
    virtual rt_status initCheck() = 0;
    virtual rt_status prepare() = 0;
    virtual rt_status prepareAsync() = 0;
    virtual rt_status seekTo(int64_t usec) = 0;
    virtual rt_status start() = 0;
    virtual rt_status stop() = 0;
    virtual rt_status pause() = 0;
    virtual rt_status reset() = 0;
    virtual rt_status wait(int64_t timeUs = 0) = 0;  // waiting until playback done.

    /*
     * basic query operations
     */
    virtual rt_status getState() = 0;
    virtual rt_status getCurrentPosition(int64_t *usec) = 0;
    virtual rt_status getDuration(int64_t *usec) = 0;
    virtual rt_status dump(int fd, const char* args) = 0;

    /*
     * advanced query/cotrol operations
     *   Parcel is defined in /android/frameworks/native/libs/binder/Parcel.cpp
     * RTParcel is defined in /src/rt_media/sink-osal/RTParcel.cpp
     */
    virtual rt_status invoke(const RtMetaData *request, RtMetaData *reply) = 0;
    virtual rt_status setParameter(int key, const RTParcel &request) = 0;
    virtual rt_status getParameter(int key, RTParcel *reply) = 0;

    /*
     * basic operations for audiotrack
     * attachAuxEffect: attaches an auxiliary effect to the audio track
     */
    virtual rt_status setAudioSink(const void* audioSink) = 0;

    virtual rt_status setVideoSurface(void* surface) = 0;
    virtual rt_status setVideoSurfaceCB(void *callback) = 0;
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_PLAYER_RTNDKMEDIAPLAYER_H_
