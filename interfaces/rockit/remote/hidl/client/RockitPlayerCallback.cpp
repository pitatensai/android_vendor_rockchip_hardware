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
#define LOG_TAG "RTPlayerCallback"

#include <utils/Log.h>
#include "include/RockitPlayerCallback.h"
#include "include/RockitPlayerInterface.h"

#define RT_KEY_LOCAL_SETTING                 102
#define RT_KEY_START_TIME                    7
#define RT_KEY_STRUCT_TEXT                   16
#define RT_TEXT_NOTIFY_MSG                   99

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {

using ::android::Parcel;

RockitPlayerCallback::RockitPlayerCallback(android::MediaPlayerInterface *player) {
    mPlayer = player;
}

RockitPlayerCallback::~RockitPlayerCallback() {
    mPlayer = NULL;
    ALOGV("~RockitPlayerCallback(%p) destruct", this);
}


Return<void> RockitPlayerCallback::sendEvent(int32_t msg, int32_t ext1, int32_t ext2, const NotifyTimeTextInfo & obj) {
    if(msg == RT_TEXT_NOTIFY_MSG) {
        Parcel txtParcel;
        txtParcel.writeInt32(RT_KEY_LOCAL_SETTING);
        txtParcel.writeInt32(RT_KEY_START_TIME);
        txtParcel.writeInt32((int32_t)obj.startTime);
        txtParcel.writeInt32(RT_KEY_STRUCT_TEXT);
        txtParcel.writeInt32(obj.size);
        txtParcel.writeByteArray(obj.size, (const uint8_t *)obj.text.c_str());
        mPlayer->sendEvent(msg, ext1, ext2, &txtParcel);
        txtParcel.freeData();
    } else {
        mPlayer->sendEvent(msg, ext1, ext2, NULL);
    }
    return Void();
}

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware


