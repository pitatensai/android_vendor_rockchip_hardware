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

//#define LOG_NDEBUG 0
#define LOG_TAG "RTNativeWindowCallback"

#include <gralloc_priv_omx.h>
#include <string.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <system/window.h>
#include <gui/Surface.h>
#include "RTNativeWindowCallback.h"
#include "RockitPlayer.h"
#include "drm.h"
#include "drm_mode.h"

using namespace ::android;

static const char *DRM_DEV_NAME = "/dev/dri/card0";

int32_t drm_open() {
    int32_t fd = open(DRM_DEV_NAME, O_RDWR);
    if (fd < 0) {
        ALOGE("open %s failed!\n", DRM_DEV_NAME);
    }

    return fd;
}

int32_t drm_close(int32_t fd) {
    int32_t ret = close(fd);
    if (ret < 0) {
        return -errno;
    }

    return ret;
}

int drm_ioctl(int32_t fd, int32_t req, void* arg) {
    int32_t ret = ioctl(fd, req, arg);
    if (ret < 0) {
        ALOGE("fd: %d ioctl %x failed with code %d: %s\n", fd, req, ret, strerror(errno));
        return -errno;
    }
    return ret;
}

int drm_free(int fd, uint32_t handle) {
    struct drm_mode_destroy_dumb data = {
        .handle = handle,
    };
    return drm_ioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &data);
}

int drm_get_phys(int fd, uint32_t handle, uint32_t *phy, uint32_t heaps) {
    (void)heaps;
    struct drm_rockchip_gem_phys phys_arg;
    phys_arg.handle = handle;
    int ret = drm_ioctl(fd, DRM_IOCTL_ROCKCHIP_GEM_GET_PHYS, &phys_arg);
    if (ret < 0) {
        ALOGE("drm_get_phys failed ret = %d", ret);
        return ret;
    } else {
        *phy = phys_arg.phy_addr;
    }
    return ret;
}

int32_t drm_fd_to_handle(
        int32_t fd,
        int32_t map_fd,
        uint32_t *handle,
        uint32_t flags) {
    int32_t ret;
    struct drm_prime_handle dph;

    dph.fd = map_fd;
    dph.flags = flags;

    ret = drm_ioctl(fd, DRM_IOCTL_PRIME_FD_TO_HANDLE, &dph);
    if (ret < 0) {
        ALOGE("DRM_IOCTL_PRIME_FD_TO_HANDLE failed!");
        return ret;
    }

    *handle = dph.handle;

    return ret;
}

RTNativeWindowCallback::RTNativeWindowCallback()
    : mDrmFd(-1) {
    if (mDrmFd < 0) {
        mDrmFd = drm_open();
    }
}

RTNativeWindowCallback::~RTNativeWindowCallback() {
    ALOGD("~RTNativeWindowCallback(%p) construct", this);
    if (mDrmFd >= 0) {
        drm_close(mDrmFd);
        mDrmFd = -1;
    }
}

int RTNativeWindowCallback::setCrop(
        void *nativeWindow,
        int32_t left,
        int32_t top,
        int32_t right,
        int32_t bottom) {
        ALOGV("%s %d in", __FUNCTION__, __LINE__);
        android_native_rect_t crop;

        crop.left = left;
        crop.top = top;
        crop.right = right;
        crop.bottom = bottom;
        return native_window_set_crop((ANativeWindow *)nativeWindow,&crop);
}

int RTNativeWindowCallback::setBufferCount(void *nativeWindow, int32_t bufferCount) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_buffer_count((ANativeWindow *)nativeWindow, bufferCount);
}

int RTNativeWindowCallback::connect(void *nativeWindow, int32_t mode) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    (void)mode;
    return native_window_api_connect((ANativeWindow *)nativeWindow, NATIVE_WINDOW_API_MEDIA);
}

int RTNativeWindowCallback::disconnect(void *nativeWindow, int32_t mode) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    (void)mode;
    return native_window_api_disconnect((ANativeWindow *)nativeWindow, NATIVE_WINDOW_API_MEDIA);;
}

int RTNativeWindowCallback::setScalingMode(void *nativeWindow, int32_t mode) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_scaling_mode((ANativeWindow *)nativeWindow, mode);;
}

int RTNativeWindowCallback::setUsage(void *nativeWindow, int32_t usage) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_usage((ANativeWindow *)nativeWindow,  usage);;
}

int RTNativeWindowCallback::query(void *nativeWindow, int32_t cmd, int32_t *param) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return ((ANativeWindow*)nativeWindow)->query((ANativeWindow *)nativeWindow, cmd, param);

}

int RTNativeWindowCallback::setBufferGeometry(
        void *nativeWindow,
        int32_t width,
        int32_t height,
        int32_t format) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    native_window_set_buffers_dimensions((ANativeWindow *)nativeWindow, width, height);
    native_window_set_buffers_format((ANativeWindow *)nativeWindow, format);

    return 0;
}

int RTNativeWindowCallback::dequeueBufferAndWait(void *nativeWindow, RTNativeWindowBufferInfo *info) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    int                         ret = 0;
    ANativeWindowBuffer*        buf;
    gralloc_private_handle_t    priv_hnd;
    ret = native_window_dequeue_buffer_and_wait((ANativeWindow *)nativeWindow,&buf);

    sp<android::GraphicBuffer> graphicBuffer(android::GraphicBuffer::from(buf));
    Rockchip_get_gralloc_private((uint32_t *)buf->handle, &priv_hnd);
    uint32_t handle = 0;
    struct drm_gem_flink req;

    if (mDrmFd >= 0) {
        drm_fd_to_handle(mDrmFd, priv_hnd.share_fd, &handle, 0);
        /* Flink creates a name for the object and returns it to the
         * application. This name can be used by other applications to gain
         * access to the same object. */
        req.handle = handle,
        drm_ioctl(mDrmFd, DRM_IOCTL_GEM_FLINK, &req);
        /*
         * must close handle here, or lead to drm buffer leak
         */
        //drm_free(mDrmFd, handle);
    }

    info->graphicBuffer = (void *)(graphicBuffer.get());
    info->window = (void*)buf->handle;
    info->windowBuf = buf;
    info->name = req.name;
    info->dupFd = priv_hnd.share_fd;

    return ret;
}

int RTNativeWindowCallback::queueBuffer(void *nativeWindow, void *buf, int32_t fence) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return ((ANativeWindow*)nativeWindow)->queueBuffer((ANativeWindow *)nativeWindow, (ANativeWindowBuffer *)buf, fence);
}

int RTNativeWindowCallback::dequeueBuffer(void *nativeWindow, void **buf) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    (void)nativeWindow;
    (void)buf;
    return 0;

}

int RTNativeWindowCallback::cancelBuffer(void *nativeWindow, void *buf, int32_t fence) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return ((ANativeWindow*)nativeWindow)->cancelBuffer((ANativeWindow *)nativeWindow, (ANativeWindowBuffer *)buf, fence);
}

int RTNativeWindowCallback::setSwapInterval(void *nativeWindow, int32_t isInterval) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    (void)nativeWindow;
    (void)isInterval;
    return 0;
}

int RTNativeWindowCallback::setDataSpace(void *nativeWindow, int32_t dataSpace) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_buffers_data_space((ANativeWindow *)nativeWindow,(android_dataspace_t)dataSpace);
}

int RTNativeWindowCallback::setTransform(void *nativeWindow, int32_t transform) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_buffers_transform((ANativeWindow *)nativeWindow, transform);
}
