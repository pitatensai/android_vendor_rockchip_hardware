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

#define LOG_NDEBUG 0
#define LOG_TAG "RockitPlayerService"

#include <dlfcn.h>
#include <android-base/logging.h>
#include <hidl/HidlBinderSupport.h>
#include "RTUtils.h"
#include "RockitPlayer.h"
#include "RockitPlayerService.h"
#include "hwbinder/Parcel.h"

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {
namespace utils {

using namespace ::android;
using namespace ::android::hardware;

RockitPlayerService::RockitPlayerService()
{
    
}

RockitPlayerService::~RockitPlayerService() {
    ALOGV("RockitPlayerService::~RockitPlayerService");
}

Return<void> RockitPlayerService::createPlayer(createPlayer_cb _hidl_cb)
{
    sp<IRockitPlayer> p = new RockitPlayer();
    Status ret = p->createPlayer();
    if (ret == Status::OK) {
        _hidl_cb(Status::OK, p);
        addClient(p);
    } else {
        _hidl_cb(Status::CORRUPTED, p);
    }

    return Void();
}

Return<Status> RockitPlayerService::destroyPlayer(const sp<IRockitPlayer>& player)
{
    if (player.get() != NULL) {
        player->destroyPlayer();
        removeClient(player);
        return Status::OK;
    } else {
        return Status::BAD_VALUE;
    }
}

void RockitPlayerService::addClient(const sp<IRockitPlayer>& client)
{
    Mutex::Autolock lock(mLock);
    mClients.add(client);
}

void RockitPlayerService::removeClient(const sp<IRockitPlayer>& client)
{
    Mutex::Autolock lock(mLock);
    mClients.remove(client);
}


}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware
}  // namespace rockchip

