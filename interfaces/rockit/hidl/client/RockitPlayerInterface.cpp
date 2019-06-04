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

#define LOG_NDEBUG 0
#define LOG_TAG "RockitPlayerClient"
#include "include/RockitPlayerInterface.h"
#include "include/RockitPlayerManager.h"

#include <utils/Log.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <media/Metadata.h>
#include <media/MediaHTTPService.h>
#include <media/mediaplayer.h>
#include <gui/Surface.h>
#include <system/window.h>
#include <string.h>


#undef ALOGV
#define ALOGV ALOGE

namespace android {


RockitPlayerClient::RockitPlayerClient() {
    mPlayer = (void*)new RockitPlayerManager(this);
}

RockitPlayerClient::~RockitPlayerClient() {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    if (play != NULL) {
        delete play;
    }
    mPlayer = NULL;
}

status_t RockitPlayerClient::initCheck() {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->initCheck();
    }

    return status;
}

status_t RockitPlayerClient::setUID(uid_t uid) {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->setUID(uid);
    }

    return status;
}

status_t RockitPlayerClient::setDataSource(
        const sp<IMediaHTTPService> &httpService,
        const char *url,
        const KeyedVector<String8, String8> *headers) {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->setDataSource(httpService, url, headers);
    }

    return status;
}

// Warning: The filedescriptor passed into this method will only be valid until
// the method returns, if you want to keep it, dup it!
status_t RockitPlayerClient::setDataSource(int fd, int64_t offset, int64_t length) {
    ALOGV("setDataSource(%d, %lld, %lld)", fd, (long long)offset, (long long)length);
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer; 
    status_t status = OK;
    if (play != NULL) {
        status = play->setDataSource(fd, offset, length);
    }

    return status;
}

status_t RockitPlayerClient::setDataSource(const sp<IStreamSource> &source) {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        play->setDataSource(source);
    }

    return status;
}

status_t RockitPlayerClient::setVideoSurfaceTexture(
        const sp<IGraphicBufferProducer> &bufferProducer) {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->setVideoSurfaceTexture(bufferProducer);
    }

    return status;
}

status_t RockitPlayerClient::prepare() {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->prepare();
    }

    return status;
}

status_t RockitPlayerClient::prepareAsync() {
    ALOGV("prepareAsync");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->prepareAsync();
    }

    return status;
}

status_t RockitPlayerClient::start() {
    ALOGV("start");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->start();
    }

    return status;
}

status_t RockitPlayerClient::stop() {
    ALOGV("stop");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->stop();
    }

    return status;
}

status_t RockitPlayerClient::pause() {
    ALOGV("pause");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->pause();
    }

    return status;
}

bool RockitPlayerClient::isPlaying() {
    ALOGV("isPlaying");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    bool playing = false;
    if (play != NULL) {
        playing = play->isPlaying();
    }

    return playing;
}

status_t RockitPlayerClient::seekTo(int msec, MediaPlayerSeekMode mode) {
    ALOGV("seekTo %.2f secs", msec / 1E3);
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->seekTo(msec, mode);
    }

    return status;
}

status_t RockitPlayerClient::getCurrentPosition(int *msec) {
    ALOGV("getCurrentPosition");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->getCurrentPosition(msec);
    }

    return status;
}

status_t RockitPlayerClient::getDuration(int *msec) {
    ALOGV("getDuration");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->getDuration(msec);
    }

    return status;
}

status_t RockitPlayerClient::reset() {
    ALOGV("reset");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->reset();
    }

    return status;
}

status_t RockitPlayerClient::setLooping(int loop) {
    ALOGV("setLooping");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->setLooping(loop);
    }

    return status;
}

player_type RockitPlayerClient::playerType() {
    ALOGV("playerType");
    return ROCKIT_PLAYER;
}

status_t RockitPlayerClient::invoke(const Parcel &request, Parcel *reply) {
    ALOGV("invoke");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->invoke(request, reply);
    }

    return status;
}

void RockitPlayerClient::setAudioSink(const sp<AudioSink> &audioSink) {
    ALOGV("setAudioSink audiosink: %p", audioSink.get());
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    if (play != NULL) {
        play->setAudioSink(audioSink);
    }
}

status_t RockitPlayerClient::setParameter(int key, const Parcel &request) {
    ALOGV("setParameter(key=%d)", key);
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->setParameter(key, request);
    }

    return status;
}

status_t RockitPlayerClient::getParameter(int key, Parcel *reply) {
    ALOGV("getParameter");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->getParameter(key, reply);
    }

    return status;
}

status_t RockitPlayerClient::getMetadata(
        const media::Metadata::Filter& ids, Parcel *records) {
    ALOGV("getMetadata");
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->getMetadata(ids, records);
    }

    return status;
}

status_t RockitPlayerClient::getPlaybackSettings(AudioPlaybackRate* rate) {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->getPlaybackSettings(rate);
    }

    return status;
}

status_t RockitPlayerClient::setPlaybackSettings(const AudioPlaybackRate& rate) {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->setPlaybackSettings(rate);
    }

    return status;
}

status_t RockitPlayerClient::dump(int fd, const Vector<String16> &args) const {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    status_t status = OK;
    if (play != NULL) {
        status = play->dump(fd, args);
    }

    return status;
}

sp<MediaPlayerBase::AudioSink> RockitPlayerClient::getAudioSink() {
    RockitPlayerManager* play = (RockitPlayerManager*)mPlayer;
    if (play != NULL) {
        return play->getAudioSink();
    } else {
        return NULL;
    }
}

}  // namespace android


