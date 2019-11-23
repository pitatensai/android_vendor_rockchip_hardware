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

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {

RockitPlayerCallback::RockitPlayerCallback(android::MediaPlayerInterface *player) {
    mPlayer = player;
}

RockitPlayerCallback::~RockitPlayerCallback() {
    mPlayer = NULL;
    ALOGV("~RockitPlayerCallback(%p) destruct", this);
}

Return<void> RockitPlayerCallback::sendEvent(int32_t msg, int32_t ext1, int32_t ext2) {
    mPlayer->sendEvent(msg, ext1, ext2);
    return Void();
}

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware


