/*
**
** Copyright 2009, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef _NATIVEWINDOWCALLBACK_H_
#define _NATIVEWINDOWCALLBACK_H_

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <media/MediaPlayerInterface.h>
#include <system/audio-base.h>

#include <rockchip/hardware/rockit/1.0/IRTNativeWindowCallback.h>

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

typedef struct RT_NATIVEWINDOW_BUFFER_INFO {
    int32_t         dupFd;
    int32_t         name;   // this name can be used by other process
    int32_t         size;
    unsigned long   phyAddr;
    void           *graphicBuffer;
    void           *window;
    void           *windowBuf;
} RTNativeWindowBufferInfo;

class RTNativeWindow : public IRTNativeWindowCallback {
public:
    RTNativeWindow();
    virtual ~RTNativeWindow();

    virtual Return<Status> setCrop(hidl_vec<uint8_t> const& nativeWindow,
                                    int32_t left,
                                    int32_t top,
                                    int32_t right,
                                    int32_t bottom);

    virtual Return<Status> setBufferCount(hidl_vec<uint8_t> const& nativeWindow, int32_t bufferCount);

    virtual Return<Status> apiConnect(hidl_vec<uint8_t> const& nativeWindow, int32_t mode);

    virtual Return<Status> apiDisconnect(hidl_vec<uint8_t> const& nativeWindow, int32_t mode);

    virtual Return<Status> setScalingMode(hidl_vec<uint8_t> const& nativeWindow, int32_t mode);

    virtual Return<Status> setUsage(hidl_vec<uint8_t> const& nativeWindow, int32_t usage);

    virtual Return<void> query(hidl_vec<uint8_t> const& nativeWindow, int32_t cmd, query_cb _hidl_cb);

    virtual Return<Status> setBuffersGeometry(hidl_vec<uint8_t> const& nativeWindow,
                                                    int32_t width,
                                                    int32_t height,
                                                    int32_t format);

    virtual Return<void> dequeueBufferAndWait(hidl_vec<uint8_t> const& nativeWindow,
                                                        dequeueBufferAndWait_cb _hidl_cb);

    virtual Return<Status> queueBuffer(hidl_vec<uint8_t> const& nativeWindow,
                                            hidl_vec<uint8_t> const& buf,
                                            int32_t fence);

    virtual Return<void> dequeueBuffer(hidl_vec<uint8_t> const& nativeWindow, dequeueBuffer_cb _hidl_cb);

    virtual Return<Status> cancelBuffer(hidl_vec<uint8_t> const& nativeWindow,
                                             hidl_vec<uint8_t> const& buf,
                                             int32_t fence);

    virtual Return<Status> setSwapInterval(hidl_vec<uint8_t> const& nativeWindow,
                                                 int32_t isInterval);

    virtual Return<Status> setDataSpace(hidl_vec<uint8_t> const& nativeWindow,
                                             int32_t dataSpace);
    virtual Return<void>   setNativeWindow(ANativeWindow *window);
private:
    int32_t             mDrmFd;
    ANativeWindow      *mWindow;
};

}  // namespace utils
}  // namespace V1_0
}  // namespace rockit
}  // namespace hardware


#endif  // _ROCKITPLAYERCALLBACK_H_
