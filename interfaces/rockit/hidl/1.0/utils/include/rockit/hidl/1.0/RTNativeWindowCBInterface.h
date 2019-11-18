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
 * author: rimon.xu@rock-chips.com
 *   date: 2019/06/17
 */

#ifndef ROCKIT_HIDL_V1_0_UTILS_RTNATIVEWINDOWCBINTERFACE_H_
#define ROCKIT_HIDL_V1_0_UTILS_RTNATIVEWINDOWCBINTERFACE_H_

#include <sys/types.h>
#include <inttypes.h>

typedef struct RT_NATIVEWINDOW_BUFFER_INFO {
    int32_t         dupFd;
    int32_t         name;   // this name can be used by other process
    int32_t         size;
    unsigned long   phyAddr;
    void           *graphicBuffer;
    void           *window;
    void           *windowBuf;
} RTNativeWindowBufferInfo;

class RTNativeWindowCBInterface {
 public:
    virtual int setCrop(void *nativeWindow, int32_t left, int32_t top, int32_t right, int32_t bottom) = 0;

    virtual int setBufferCount(void *nativeWindow, int32_t bufferCount) = 0;

    virtual int connect(void *nativeWindow, int32_t mode) = 0;

    virtual int disconnect(void *nativeWindow, int32_t mode) = 0;

    virtual int setScalingMode(void *nativeWindow, int32_t mode) = 0;

    virtual int setUsage(void *nativeWindow, int32_t usage) = 0;

    virtual int query(void *nativeWindow, int32_t cmd, int32_t *param) = 0;

    virtual int setBufferGeometry(void *nativeWindow, int32_t width, int32_t height, int32_t format) = 0;

    virtual int dequeueBufferAndWait(void *nativeWindow, RTNativeWindowBufferInfo *info) = 0;

    virtual int queueBuffer(void *nativeWindow, void *buf, int32_t fence) = 0;

    virtual int dequeueBuffer(void *nativeWindow, void **buf) = 0;

    virtual int cancelBuffer(void *nativeWindow, void *buf, int32_t fence) = 0;

    virtual int setSwapInterval(void *nativeWindow, int32_t isInterval) = 0;

    virtual int setDataSpace(void *nativeWindow, int32_t dataSpace) = 0;

    virtual int setTransform(void *nativeWindow, int32_t transform) = 0;

    virtual ~RTNativeWindowCBInterface() {}
};


#endif  // ROCKIT_HIDL_V1_0_UTILS_RTNATIVEWINDOWCBINTERFACE_H_

