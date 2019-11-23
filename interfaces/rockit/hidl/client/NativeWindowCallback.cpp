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
#define LOG_TAG "NativeWindowCallback"
#include <utils/Log.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <string.h>
#include <system/window.h>
#include <gralloc_priv_omx.h>

#include <ui/GraphicBuffer.h>

#include "include/NativeWindowCallback.h"
#include "include/RockitPlayerInterface.h"
#include "drm.h"
#include "drm_mode.h"

namespace rockchip {
namespace hardware {
namespace rockit {
namespace V1_0 {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;

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

RTNativeWindow::RTNativeWindow()
        : mDrmFd(-1) {
    ALOGV("RTNativeWindow::RTNativeWindow");
    if (mDrmFd < 0) {
        mDrmFd = drm_open();
    }
}

RTNativeWindow::~RTNativeWindow() {
    mWindow = NULL;
    ALOGV("RTNativeWindow::~RTNativeWindow");
    if (mDrmFd >= 0) {
        drm_close(mDrmFd);
        mDrmFd = -1;
    }
}


Return<Status> RTNativeWindow::setCrop(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t left,
        int32_t top,
        int32_t right,
        int32_t bottom) {
    android_native_rect_t   crop;
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window      = ((ANativeWindow **)hidlWindow.data())[0];
    if (mWindow != NULL) {
        crop.left   = left;
        crop.top    = top;
        crop.right  = right;
        crop.bottom = bottom;
        ALOGV("setCrop window: %p, left: %d, top: %d, right: %d, bottom: %d",
               mWindow, crop.left, crop.top, crop.right, crop.bottom);
        native_window_set_crop(mWindow, &crop);
    }

    return Status::OK;
}

Return<Status> RTNativeWindow::setBufferCount(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t bufferCount) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];

    if (mWindow != NULL) {
        ALOGV("setBufferCount window: %p, bufferCount: %d", mWindow, bufferCount);
        native_window_set_buffer_count(mWindow, bufferCount);
    }
    return Status::OK;
}

Return<Status> RTNativeWindow::apiConnect(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t mode) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    if (mDrmFd < 0) {
        mDrmFd = drm_open();
    }

    window = ((ANativeWindow **)hidlWindow.data())[0];
    if (mWindow != NULL) {
        ALOGV("apiConnect window: %p, mode: %d", mWindow, mode);
        native_window_api_connect(mWindow, mode);
    }
    return Status::OK;
}

Return<Status> RTNativeWindow::apiDisconnect(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t mode) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    if (mDrmFd >= 0) {
        drm_close(mDrmFd);
        mDrmFd = -1;
    }

    window = ((ANativeWindow **)hidlWindow.data())[0];
    if (mWindow != NULL) {
        ALOGV("apiDisconnect window: %p, mode: %d", mWindow, mode);
        native_window_api_disconnect(mWindow, mode);
    }
    return Status::OK;
}

Return<Status> RTNativeWindow::setScalingMode(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t mode) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    if (mWindow != NULL) {
        ALOGV("setScalingMode window: %p, mode: %d", mWindow, mode);
        native_window_set_scaling_mode(mWindow, mode);
    }
    return Status::OK;
}

Return<Status> RTNativeWindow::setUsage(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t usage) {
    ANativeWindow          *window;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    if (mWindow != NULL) {
        ALOGV("setUsage window: %p, usage: 0x%x", mWindow, usage);
        native_window_set_usage(mWindow, usage);
    }
    return Status::OK;
}

Return<void> RTNativeWindow::query(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t cmd,
        query_cb _hidl_cb) {
    int32_t                 param  = 0;
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    if (mWindow != NULL) {
        mWindow->query(window, cmd, &param);
    }
    ALOGV("query window: %p, cmd: %d, param: %d", mWindow, cmd, param);
    _hidl_cb(Status::OK, param);
    return Void();
}

Return<Status> RTNativeWindow::setBuffersGeometry(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t width,
        int32_t height,
        int32_t format) {
    ANativeWindow          *window;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    ALOGV("setBuffersGeometry window: %p, width: %d, height: %d, format: %d",
           mWindow, width, height, format);
    if (mWindow != NULL) {
        native_window_set_buffers_dimensions(mWindow, width, height);
        native_window_set_buffers_format(mWindow, format);
    }
    return Status::OK;
}

Return<void> RTNativeWindow::dequeueBufferAndWait(
        hidl_vec<uint8_t> const& nativeWindow,
        dequeueBufferAndWait_cb _hidl_cb) {
    ANativeWindowBuffer        *buf    = NULL;
    ANativeWindow              *window = NULL;
    hidl_vec<uint8_t>           hidlWindow(nativeWindow);
    hidl_vec<uint8_t>           hidlInfo;
    RTNativeWindowBufferInfo    info;
    gralloc_private_handle_t        priv_hnd;

    window = ((ANativeWindow **)hidlWindow.data())[0];
    ALOGV("dequeueBufferAndWait window: %p", mWindow);
    if (mWindow != NULL) {
        native_window_dequeue_buffer_and_wait(mWindow, &buf);

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
        }

        info.dupFd          = 0;
        info.phyAddr        = 0;
        info.window         = mWindow;
        info.size           = 0;
        info.name           = req.name;
        info.graphicBuffer  = (void *)(graphicBuffer.get());
        info.windowBuf      = (void *)buf;
    }

    void *tmpInfo = (void *)&info;
    hidlInfo.setToExternal(static_cast<uint8_t *>(tmpInfo), sizeof(RTNativeWindowBufferInfo), false);
    ALOGV("dequeueBufferAndWait window: %p", window);
    _hidl_cb(Status::OK, hidlInfo);
    return Void();
}

Return<Status> RTNativeWindow::queueBuffer(
        hidl_vec<uint8_t> const& nativeWindow,
        hidl_vec<uint8_t> const& buf,
        int32_t fence) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);
    hidl_vec<uint8_t>       hidlBuf(buf);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    ALOGV("queueBuffer window: %p buf: %p", mWindow, ((ANativeWindowBuffer **)hidlBuf.data())[0]);
    if (mWindow != NULL) {
        mWindow->queueBuffer(window, ((ANativeWindowBuffer **)hidlBuf.data())[0], fence);
    }
    return Status::OK;
}

Return<void> RTNativeWindow::dequeueBuffer(
        hidl_vec<uint8_t> const& nativeWindow,
        dequeueBuffer_cb _hidl_cb) {
    hidl_vec<uint8_t>       hidlBuf;
    int32_t                 fence   = 0;
    ANativeWindowBuffer    *buf     = NULL;
    ANativeWindow          *window  = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];

    if (mWindow != NULL) {
        // TODO(): should send fence
        mWindow->dequeueBuffer(mWindow, &buf, &fence);
    }

    void *tmpBuf = (void *)&buf;
    hidlBuf.setToExternal(static_cast<uint8_t *>(tmpBuf), sizeof(void *), false);
    ALOGV("dequeueBuffer window: %p", mWindow);
    _hidl_cb(Status::OK, hidlBuf);
    return Void();
}

Return<Status> RTNativeWindow::cancelBuffer(
        hidl_vec<uint8_t> const& nativeWindow,
        hidl_vec<uint8_t> const& buf,
        int32_t fence) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);
    hidl_vec<uint8_t>       hidlBuf(buf);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    if (mWindow != NULL) {
        mWindow->cancelBuffer(mWindow, ((ANativeWindowBuffer **)hidlBuf.data())[0], fence);
    }
    return Status::OK;
}

Return<Status> RTNativeWindow::setSwapInterval(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t isInterval) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    ALOGV("setSwapInterval window: %p, isInterval: %d", mWindow, isInterval);
    if (mWindow != NULL) {
        mWindow->setSwapInterval(mWindow, isInterval);
    }
    return Status::OK;
}

Return<Status> RTNativeWindow::setDataSpace(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t dataSpace) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    ALOGV("setDataSpace window: %p, dataSpace: 0x%x", mWindow, dataSpace);
    if (mWindow != NULL) {
        native_window_set_buffers_data_space(mWindow, (android_dataspace_t)dataSpace);
    }
    return Status::OK;
}

Return<Status> RTNativeWindow::setTransform(
        hidl_vec<uint8_t> const& nativeWindow,
        int32_t transform) {
    ANativeWindow          *window = NULL;
    hidl_vec<uint8_t>       hidlWindow(nativeWindow);

    window = ((ANativeWindow **)hidlWindow.data())[0];
    ALOGV("setTransform  window: %p, transform: 0x%x", mWindow, transform);
    if (mWindow != NULL) {
        native_window_set_buffers_transform(mWindow, transform);
    }
    return Status::OK;
}


Return<void> RTNativeWindow::setNativeWindow(ANativeWindow *window) {
    mWindow = window;
    return Void();
}

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware


