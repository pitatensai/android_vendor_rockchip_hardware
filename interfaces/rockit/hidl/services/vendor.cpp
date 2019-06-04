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
#define LOG_TAG "rockchip.hardware.rockit@1.0-service"

#include "RockitPlayer.h"

#include <hidl/HidlTransportSupport.h>
#include <binder/ProcessState.h>
#include <minijail.h>

// This is created by module "codec2.vendor.base.policy". This can be modified.
static constexpr char kBaseSeccompPolicyPath[] =
        "/vendor/etc/seccomp_policy/rockit.vendor.base.policy";

// Additional device-specific seccomp permissions can be added in this file.
static constexpr char kExtSeccompPolicyPath[] =
        "/vendor/etc/seccomp_policy/rockit.vendor.ext.policy";

int main(int /* argc */, char** /* argv */) {
    ALOGD("rockit.hardware.rockit@1.0-service starting...");

    signal(SIGPIPE, SIG_IGN);
    android::SetUpMinijail(kBaseSeccompPolicyPath, kExtSeccompPolicyPath);

    // vndbinder is needed by BufferQueue.
    android::ProcessState::initWithDriver("/dev/vndbinder");
    android::ProcessState::self()->startThreadPool();

    // Extra threads may be needed to handle a stacked IPC sequence that
    // contains alternating binder and hwbinder calls. (See b/35283480.)
    android::hardware::configureRpcThreadpool(8, true /* callerWillJoin */);

    // TODO(): rockit service.
    {
        using namespace ::rockchip::hardware::rockit::V1_0;
        android::sp<IRockitPlayer> player;
        android::status_t status;

        // Vendor's TODO: Replace this with
        ALOGD("Instantiating Rockit player service...");
        player = new utils::RockitPlayer();

        if (player == nullptr) {
            ALOGE("Cannot create Rockit player service.");
        } else {
            status = player->registerAsService();
            if (status != android::OK) {
                ALOGE("Cannot register Rockit player service.");
            } else {
                ALOGI("Rockit player service created.");
            }
        }
    }

    android::hardware::joinRpcThreadpool();
    return 0;
}
