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
 * author: hh@rock-chips.com
 *   date: 2019/10/09
 * module: RTMedaDataInterface
 */

#ifndef SRC_RT_METAPDATAINTERFACE_H_
#define SRC_RT_METAPDATAINTERFACE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <inttypes.h>


enum RTBool {
    RT_FLASE = 0,
    RT_TRUE = 1
};

class RtMetaDataInterface {
 public:
    RtMetaDataInterface(){};
    virtual ~RtMetaDataInterface(){};

    virtual void clear() = 0;
    virtual RTBool remove(uint32_t key) = 0;

    virtual RTBool setCString(uint32_t key, const char *value) = 0;
    virtual RTBool setInt32(uint32_t key, int32_t value) = 0;
    virtual RTBool setInt64(uint32_t key, int64_t value) = 0;
    virtual RTBool setFloat(uint32_t key, float value) = 0;
    virtual RTBool setPointer(uint32_t key, void* value) = 0;

    virtual RTBool findCString(uint32_t key, const char **value) const = 0;
    virtual RTBool findInt32(uint32_t key, int32_t *value) const = 0;
    virtual RTBool findInt64(uint32_t key, int64_t *value) const = 0;
    virtual RTBool findFloat(uint32_t key, float *value) const = 0;
    virtual RTBool findPointer(uint32_t key, void **value) const = 0;

    virtual RTBool setData(uint32_t key, uint32_t type, const void *data, uint32_t size) = 0;

    virtual RTBool findData(uint32_t key, uint32_t *type,
                  const void **data, uint32_t *size) const = 0;

    virtual RTBool hasData(uint32_t key) const = 0;

    void dumpToLog() const;
};

#define MKTAG(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

/* must keep sync with rockit */
enum {
    kUserInvokeCmd                = MKTAG('u', 'c', 'm', 'd'),
    kUserInvokeTracksCount        = MKTAG('t', 'c', 'n', 't'),
    kUserInvokeTracksInfor        = MKTAG('t', 'i', 'n', 'f'),
    kUserInvokeSetTrackIdx        = MKTAG('t', 'i', 'd', 'x'),
    kUserInvokeVideoScallingMode  = MKTAG('v', 's', 'm', 'd'),
    kUserInvokeGetSelectTrack     = MKTAG('t', 's', 'l', 'e'),
    kUserInvokeGetSelectTrackIdx  = MKTAG('t', 'i', 'd', 'x'),

    kUserInvokeSetPlaybackRate    = MKTAG('i', 'p', 'b', 'r'), // float

    /* user's notify command*/
    kUserNotifyPts                = MKTAG('u', 'n', 'p', 's'),
    kUserNotifySize               = MKTAG('u', 'n', 's', 'e'),
    kUserNotifyData               = MKTAG('u', 'n', 'd', 'a'),
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_METAPDATAINTERFACE_H_
