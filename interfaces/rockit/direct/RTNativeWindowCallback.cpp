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
#define LOG_TAG "RTNativeWindowCallback"

#include <gralloc_priv_omx.h>
#include <string.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <system/window.h>
#include <gui/Surface.h>
#include "RTNativeWindowCallback.h"
#include "RockitPlayer.h"

using namespace ::android;

RTNativeWindowCallback::RTNativeWindowCallback() {
}

RTNativeWindowCallback::~RTNativeWindowCallback() {

}

int RTNativeWindowCallback::setCrop(
        void *nativeWindow,
        int32_t left,
        int32_t top,
        int32_t right,
        int32_t bottom) {
        ALOGD("%s %d in", __FUNCTION__, __LINE__);
        android_native_rect_t crop;

        crop.left = left;
        crop.top = top;
        crop.right = right;
        crop.bottom = bottom;
        return native_window_set_crop((ANativeWindow *)nativeWindow,&crop);
}

int RTNativeWindowCallback::setBufferCount(void *nativeWindow, int32_t bufferCount) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_buffer_count((ANativeWindow *)nativeWindow, bufferCount);
}

int RTNativeWindowCallback::connect(void *nativeWindow, int32_t mode) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    (void)mode;
    return native_window_api_connect((ANativeWindow *)nativeWindow, NATIVE_WINDOW_API_MEDIA);
}

int RTNativeWindowCallback::disconnect(void *nativeWindow, int32_t mode) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    (void)mode;
    return native_window_api_disconnect((ANativeWindow *)nativeWindow, NATIVE_WINDOW_API_MEDIA);;
}

int RTNativeWindowCallback::setScalingMode(void *nativeWindow, int32_t mode) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_scaling_mode((ANativeWindow *)nativeWindow, mode);;
}

int RTNativeWindowCallback::setUsage(void *nativeWindow, int32_t usage) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_usage((ANativeWindow *)nativeWindow,  usage);;
}

int RTNativeWindowCallback::query(void *nativeWindow, int32_t cmd, int32_t *param) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    return ((ANativeWindow*)nativeWindow)->query((ANativeWindow *)nativeWindow, cmd, param);

}

int RTNativeWindowCallback::setBufferGeometry(
        void *nativeWindow,
        int32_t width,
        int32_t height,
        int32_t format) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    native_window_set_buffers_dimensions((ANativeWindow *)nativeWindow, width, height);
    native_window_set_buffers_format((ANativeWindow *)nativeWindow, format);

    return 0;
}

int RTNativeWindowCallback::dequeueBufferAndWait(void *nativeWindow, RTNativeWindowBufferInfo *info) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    int                         ret = 0;
    ANativeWindowBuffer*        buf;
    gralloc_private_handle_t    priv_hnd;
    ret = native_window_dequeue_buffer_and_wait((ANativeWindow *)nativeWindow,&buf);

    sp<android::GraphicBuffer> graphicBuffer(android::GraphicBuffer::from(buf));
    Rockchip_get_gralloc_private((uint32_t *)buf->handle, &priv_hnd);

    info->graphicBuffer = (void *)(graphicBuffer.get());
    info->window = (void*)buf->handle;
    info->windowBuf = buf;
    info->dupFd = priv_hnd.share_fd;

    return ret;
}

int RTNativeWindowCallback::queueBuffer(void *nativeWindow, void *buf, int32_t fence) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    return ((ANativeWindow*)nativeWindow)->queueBuffer((ANativeWindow *)nativeWindow, (ANativeWindowBuffer *)buf, fence);
}

int RTNativeWindowCallback::dequeueBuffer(void *nativeWindow, void **buf) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    (void)nativeWindow;
    (void)buf;
    return 0;

}

int RTNativeWindowCallback::cancelBuffer(void *nativeWindow, void *buf, int32_t fence) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    return ((ANativeWindow*)nativeWindow)->cancelBuffer((ANativeWindow *)nativeWindow, (ANativeWindowBuffer *)buf, fence);
}

int RTNativeWindowCallback::setSwapInterval(void *nativeWindow, int32_t isInterval) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    (void)nativeWindow;
    (void)isInterval;
    return 0;
}

int RTNativeWindowCallback::setDataSpace(void *nativeWindow, int32_t dataSpace) {
    ALOGD("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_buffers_data_space((ANativeWindow *)nativeWindow,(android_dataspace_t)dataSpace);
}

