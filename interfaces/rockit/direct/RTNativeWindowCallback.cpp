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
#include "sideband/RTSidebandWindow.h"
extern "C" {
#include "xf86drm.h"
#include "xf86drmMode.h"
#include "drm_fourcc.h"
}

using namespace ::android;

static const char *DRM_DEV_PATH = "/dev/dri/card0";

int32_t drm_open() {
    int32_t fd = open(DRM_DEV_PATH, O_RDWR);
    if (fd < 0) {
        ALOGE("open %s failed!\n", DRM_DEV_PATH);
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

int32_t drm_handle_to_fd(int32_t fd, uint32_t handle, int *map_fd, uint32_t flags) {
    int32_t ret;
    struct drm_prime_handle dph;
    memset(&dph, 0, sizeof(struct drm_prime_handle));
    dph.handle = handle;
    dph.fd = -1;
    dph.flags = flags;

    if (map_fd == NULL)
        return -EINVAL;

    ret = drm_ioctl(fd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &dph);
    if (ret < 0) {
        return ret;
    }

    *map_fd = dph.fd;

    if (*map_fd < 0) {
        ALOGE("fail to handle_to_fd(fd=%d)", fd);
        return -EINVAL;
    }

    return ret;
}

RTNativeWindowCallback::RTNativeWindowCallback()
    : mDrmFd(-1),
      mSidebandWindow(NULL),
      mSidebandHandle(NULL),
      mTunnel(0) {
    if (mDrmFd < 0) {
        mDrmFd = drm_open();
    }
}

RTNativeWindowCallback::~RTNativeWindowCallback() {
    ALOGD("~RTNativeWindowCallback(%p) construct", this);
    if (mSidebandHandle) {
        mSidebandWindow->freeBuffer(&mSidebandHandle);
    }
    if (mSidebandWindow.get()) {
        mSidebandWindow->release();
        mSidebandWindow.clear();
    }
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
        ALOGV("%s %d in crop(%d,%d,%d,%d)", __FUNCTION__, __LINE__, left, top, right, bottom);
        android_native_rect_t crop;

        crop.left = left;
        crop.top = top;
        crop.right = right;
        crop.bottom = bottom;
        if (mTunnel) {
            mSidebandWindow->setCrop(left, top, right, bottom);
        }
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
    ALOGV("%s %d in usage=0x%x", __FUNCTION__, __LINE__, usage);
    return native_window_set_usage((ANativeWindow *)nativeWindow, usage);;
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
    ALOGV("%s %d in width=%d, height=%d, format=0x%x", __FUNCTION__, __LINE__, width, height, format);
    native_window_set_buffers_dimensions((ANativeWindow *)nativeWindow, width, height);
    native_window_set_buffers_format((ANativeWindow *)nativeWindow, format);
    if (mTunnel) {
        mSidebandWindow->setBufferGeometry(width, height, format);
    }

    return 0;
}

int RTNativeWindowCallback::dequeueBufferAndWait(void *nativeWindow, RTNativeWindowBufferInfo *info) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    int                         ret = 0;
    buffer_handle_t             bufferHandle = NULL;
    gralloc_private_handle_t    priv_hnd;
    ANativeWindowBuffer *buf = NULL;

    memset(info, 0, sizeof(RTNativeWindowBufferInfo));
    if (mTunnel) {
        mSidebandWindow->dequeueBuffer((buffer_handle_t *)&bufferHandle);
    } else {
        ret = native_window_dequeue_buffer_and_wait((ANativeWindow *)nativeWindow, &buf);
        if (buf) {
            bufferHandle = buf->handle;
        }
    }

    if (bufferHandle) {
        Rockchip_get_gralloc_private((uint32_t *)bufferHandle, &priv_hnd);
        uint32_t handle = 0;
        struct drm_gem_flink req;

        if (mDrmFd >= 0) {
            drm_fd_to_handle(mDrmFd, priv_hnd.share_fd, &handle, 0);
            /* Flink creates a name for the object and returns it to the
             * application. This name can be used by other applications to gain
             * access to the same object. */
            req.handle = handle,
            drm_ioctl(mDrmFd, DRM_IOCTL_GEM_FLINK, &req);
            //drm_free(mDrmFd, handle);
        }

        info->graphicBuffer = NULL;
        if (mTunnel) {
            info->windowBuf = (void *)bufferHandle;
        } else {
            info->windowBuf = (void *)buf;
        }
        info->name = req.name;
        info->dupFd = priv_hnd.share_fd;
    }
    return ret;
}

int RTNativeWindowCallback::queueBuffer(void *nativeWindow, void *buf, int32_t fence) {
    ALOGV("%s %d buf=%p in", __FUNCTION__, __LINE__, buf);
    int ret = 0;
    if (mTunnel) {
        ret = mSidebandWindow->queueBuffer((buffer_handle_t)buf);
    } else {
        ret = ((ANativeWindow*)nativeWindow)->queueBuffer((ANativeWindow *)nativeWindow, (ANativeWindowBuffer *)buf, fence);
    }
    return ret;
}

int RTNativeWindowCallback::dequeueBuffer(void *nativeWindow, void **buf) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    (void)nativeWindow;
    (void)buf;
    return 0;

}

int RTNativeWindowCallback::freeBuffer(void *nativeWindow, void *buf, int32_t fence) {
    ALOGV("%s %d buf=%p in", __FUNCTION__, __LINE__, buf);
    int ret = 0;
    if (mTunnel) {
        ret = mSidebandWindow->freeBuffer((buffer_handle_t *)&buf);
    } else {
        ret = ((ANativeWindow*)nativeWindow)->cancelBuffer((ANativeWindow *)nativeWindow,
                                                            (ANativeWindowBuffer *)buf, fence);
    }

    return ret;
}

int RTNativeWindowCallback::remainBuffer(void *nativeWindow, void *buf, int32_t fence) {
    ALOGV("%s %d buf=%p in", __FUNCTION__, __LINE__, buf);
    int ret = 0;
    if (mTunnel) {
        ret = mSidebandWindow->remainBuffer((buffer_handle_t)buf);
    } else {
        ret = ((ANativeWindow*)nativeWindow)->cancelBuffer((ANativeWindow *)nativeWindow, (ANativeWindowBuffer *)buf, fence);
    }

    return ret;
}

int RTNativeWindowCallback::setSwapInterval(void *nativeWindow, int32_t isInterval) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    (void)nativeWindow;
    (void)isInterval;
    return 0;
}

int RTNativeWindowCallback::setDataSpace(void *nativeWindow, int32_t dataSpace) {
    ALOGV("%s %d in dataSpace=0x%x", __FUNCTION__, __LINE__, dataSpace);
    return native_window_set_buffers_data_space((ANativeWindow *)nativeWindow,(android_dataspace_t)dataSpace);
}

int RTNativeWindowCallback::setTransform(void *nativeWindow, int32_t transform) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);
    return native_window_set_buffers_transform((ANativeWindow *)nativeWindow, transform);
}

int RTNativeWindowCallback::allocateBuffer(void *nativeWindow, RTNativeWindowBufferInfo *info) {
    int                         ret = 0;
    buffer_handle_t             bufferHandle = NULL;
    gralloc_private_handle_t    priv_hnd;
    ANativeWindowBuffer        *buf = NULL;

    memset(info, 0, sizeof(RTNativeWindowBufferInfo));
    if (mTunnel) {
        mSidebandWindow->allocateBuffer((buffer_handle_t *)&bufferHandle);
    } else {
        ret = native_window_dequeue_buffer_and_wait((ANativeWindow *)nativeWindow, &buf);
        bufferHandle = buf->handle;
    }
    if (bufferHandle) {
        Rockchip_get_gralloc_private((uint32_t *)bufferHandle, &priv_hnd);
        uint32_t handle = 0;
        struct drm_gem_flink req;

        if (mDrmFd >= 0) {
            drm_fd_to_handle(mDrmFd, priv_hnd.share_fd, &handle, 0);
            /* Flink creates a name for the object and returns it to the
             * application. This name can be used by other applications to gain
             * access to the same object. */
            req.handle = handle,
            drm_ioctl(mDrmFd, DRM_IOCTL_GEM_FLINK, &req);
            //drm_free(mDrmFd, handle);
        }

        info->graphicBuffer = NULL;
        if (mTunnel) {
            info->windowBuf = (void *)bufferHandle;
        } else {
            info->windowBuf = (void *)buf;
        }
        info->name = req.name;
        info->dupFd = priv_hnd.share_fd;
    }

    return 0;
}

int RTNativeWindowCallback::setSidebandStream(void *nativeWindow, RTSidebandInfo info) {
    ALOGV("%s %d in", __FUNCTION__, __LINE__);

    buffer_handle_t             buffer = NULL;

    mSidebandWindow = new RTSidebandWindow();
    mSidebandWindow->init(info);
    mSidebandWindow->allocateSidebandHandle(&buffer);
    if (!buffer) {
        ALOGE("allocate buffer from sideband window failed!");
        return -1;
    }
    mSidebandHandle = buffer;
    mTunnel = 1;
    return native_window_set_sideband_stream((ANativeWindow *)nativeWindow, (native_handle_t *)buffer);
}
