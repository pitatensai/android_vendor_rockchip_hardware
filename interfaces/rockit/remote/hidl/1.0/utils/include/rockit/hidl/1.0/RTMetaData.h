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
 * author: Rimon.Xu@rock-chips.com
 *   date: 20181205
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_METADATA_H_
#define SRC_RT_BASE_INCLUDE_RT_METADATA_H_

#include <stdint.h>

struct RTMetaDataContext;

enum RTBool {
    RT_FLASE = 0,
    RT_TRUE = 1
};

#define MKTAG(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

/* must keep sync with rockit */
enum {
    kUserInvokeCmd                = MKTAG('u', 'c', 'm', 'd'),
    kUserInvokeTracksCount        = MKTAG('t', 'c', 'n', 't'),
    kUserInvokeTracksInfor        = MKTAG('t', 'i', 'n', 'f'),
    kUserInvokeTrackIdx           = MKTAG('t', 'i', 'd', 'x'),
    kUserInvokeVideoScallingMode  = MKTAG('v', 's', 'm', 'd'),
    kUserInvokeGetSelectTrack     = MKTAG('t', 's', 'l', 'e'),
    kUserInvokeGetSelectTrackIdx  = MKTAG('t', 'i', 'd', 'x'),

    /* user's notify command*/
    kUserNotifyPts                = MKTAG('u', 'n', 'p', 's'),
    kUserNotifySize               = MKTAG('u', 'n', 's', 'e'),
    kUserNotifyData               = MKTAG('u', 'n', 'd', 'a'),
};

class RtMetaData {
 public:
    RtMetaData();
    RtMetaData(const RtMetaData &from);
    RtMetaData& operator = (const RtMetaData &);

    virtual ~RtMetaData();

    virtual void clear();
    virtual RTBool remove(uint64_t key);

    virtual RTBool setCString(uint32_t key, const char *value);
    virtual RTBool setInt32(uint32_t key, int32_t value);
    virtual RTBool setInt64(uint32_t key, int64_t value);
    virtual RTBool setFloat(uint32_t key, float value);
    virtual RTBool setPointer(uint32_t key, void *value);

    virtual RTBool findCString(uint32_t key, const char **value) const;
    virtual RTBool findInt32(uint32_t key, int32_t *value) const;
    virtual RTBool findInt64(uint32_t key, int64_t *value) const;
    virtual RTBool findFloat(uint32_t key, float *value) const;
    virtual RTBool findPointer(uint32_t key, void **value) const;

    virtual RTBool setCString(const char* key, const char *value);
    virtual RTBool setInt32(const char* key, int32_t value);
    virtual RTBool setInt64(const char* key, int32_t value);
    virtual RTBool setFloat(const char* key, float value);
    virtual RTBool setPointer(const char* key, void *value);

    virtual RTBool findCString(const char* key, const char **value) const;
    virtual RTBool findInt32(const char* key, int32_t *value) const;
    virtual RTBool findInt64(const char* key, int64_t *value) const;
    virtual RTBool findFloat(const char* key, float *value) const;
    virtual RTBool findPointer(const char* key, void **value) const;

    virtual RTBool setData(uint64_t key, uint32_t type, const void *data, uint32_t size);

    virtual RTBool findData(uint64_t key, uint32_t *type,
                  const void **data, uint32_t *size) const;

    virtual RTBool hasData(uint64_t key) const;

    virtual void dumpToLog() const;

 private:
    struct              typed_data;
    RTMetaDataContext  *mCtx;
};

#endif  // SRC_RT_BASE_INCLUDE_RT_METADATA_H_

