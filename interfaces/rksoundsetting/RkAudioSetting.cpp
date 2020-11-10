/*
 * Copyright 2018 Rockchip Electronics Co. LTD
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
 */


#define LOG_TAG "RkAudioSetting-service"
#include <android-base/logging.h>
#include <log/log.h>
#include <string.h>
#include "RkAudioSettingManager.h"
#include "RkAudioSetting.h"

namespace android {

RkAudioSetting::RkAudioSetting() {
    mAudioSetting = (RkAudioSettingInterface*) new RkAudioSettingManager();
    mAudioSetting->init();
}

RkAudioSetting::~RkAudioSetting() {
    if (mAudioSetting) {
        delete(mAudioSetting);
        mAudioSetting = NULL;
    }
}

void RkAudioSetting::setSelect(int device) {
    mAudioSetting->setSelect(device);
}

void RkAudioSetting::setFormat(int device, int close, const char * format) {
    mAudioSetting->setFormat(device, close, format);
}

void RkAudioSetting::setMode(int device, int mode) {
    mAudioSetting->setMode(device, mode);
}

int RkAudioSetting::getSelect(int device) {
    int val;
    val = mAudioSetting->getSelect(device);
    return val;
}

int RkAudioSetting::getMode(int device) {
    int val;
    val = mAudioSetting->getMode(device);
    return val;
}

int RkAudioSetting::getFormat(int device, const char *format) {
    int val;
    val = mAudioSetting->getFormat(device, format);
    return val;
}

void RkAudioSetting::updataFormatForEdid() {
    mAudioSetting->updataFormatForEdid();
}

}
