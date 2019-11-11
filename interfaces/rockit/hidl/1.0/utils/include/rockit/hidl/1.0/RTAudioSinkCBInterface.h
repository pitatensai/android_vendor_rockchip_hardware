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
 *   date: 2019/06/13
 * module: android audio sink callback
 */

#ifndef ROCKIT_HIDL_V1_0_UTILS_RTAUDIOSINKCBINTERFACE_H_
#define ROCKIT_HIDL_V1_0_UTILS_RTAUDIOSINKCBINTERFACE_H_

#include <sys/types.h>
#include <inttypes.h>

typedef enum {
    RT_AUDIO_FORMAT_INVALID             = 0xFFFFFFFFu,
    RT_AUDIO_FORMAT_DEFAULT             = 0,
    RT_AUDIO_FORMAT_PCM                 = 0x00000000u,
    RT_AUDIO_FORMAT_MP3                 = 0x01000000u,
    RT_AUDIO_FORMAT_AMR_NB              = 0x02000000u,
    RT_AUDIO_FORMAT_AMR_WB              = 0x03000000u,
    RT_AUDIO_FORMAT_AAC                 = 0x04000000u,
    RT_AUDIO_FORMAT_HE_AAC_V1           = 0x05000000u,
    RT_AUDIO_FORMAT_HE_AAC_V2           = 0x06000000u,
    RT_AUDIO_FORMAT_VORBIS              = 0x07000000u,
    RT_AUDIO_FORMAT_OPUS                = 0x08000000u,
    RT_AUDIO_FORMAT_AC3                 = 0x09000000u,
    RT_AUDIO_FORMAT_E_AC3               = 0x0A000000u,
    RT_AUDIO_FORMAT_DTS                 = 0x0B000000u,
    RT_AUDIO_FORMAT_DTS_HD              = 0x0C000000u,
    RT_AUDIO_FORMAT_IEC61937            = 0x0D000000u,
    RT_AUDIO_FORMAT_DOLBY_TRUEHD        = 0x0E000000u,
    RT_AUDIO_FORMAT_EVRC                = 0x10000000u,
    RT_AUDIO_FORMAT_EVRCB               = 0x11000000u,
    RT_AUDIO_FORMAT_EVRCWB              = 0x12000000u,
    RT_AUDIO_FORMAT_EVRCNW              = 0x13000000u,
    RT_AUDIO_FORMAT_AAC_ADIF            = 0x14000000u,
    RT_AUDIO_FORMAT_WMA                 = 0x15000000u,
    RT_AUDIO_FORMAT_WMA_PRO             = 0x16000000u,
    RT_AUDIO_FORMAT_AMR_WB_PLUS         = 0x17000000u,
    RT_AUDIO_FORMAT_MP2                 = 0x18000000u,
    RT_AUDIO_FORMAT_QCELP               = 0x19000000u,
    RT_AUDIO_FORMAT_DSD                 = 0x1A000000u,
    RT_AUDIO_FORMAT_FLAC                = 0x1B000000u,
    RT_AUDIO_FORMAT_ALAC                = 0x1C000000u,
    RT_AUDIO_FORMAT_APE                 = 0x1D000000u,
    RT_AUDIO_FORMAT_AAC_ADTS            = 0x1E000000u,
    RT_AUDIO_FORMAT_SBC                 = 0x1F000000u,
    RT_AUDIO_FORMAT_APTX                = 0x20000000u,
    RT_AUDIO_FORMAT_APTX_HD             = 0x21000000u,
    RT_AUDIO_FORMAT_AC4                 = 0x22000000u,
    RT_AUDIO_FORMAT_LDAC                = 0x23000000u,
    RT_AUDIO_FORMAT_MAT                 = 0x24000000u,
    RT_AUDIO_FORMAT_AAC_LATM            = 0x25000000u,
    RT_AUDIO_FORMAT_CELT                = 0x26000000u,
    RT_AUDIO_FORMAT_APTX_ADAPTIVE       = 0x27000000u,
    RT_AUDIO_FORMAT_LHDC                = 0x28000000u,
    RT_AUDIO_FORMAT_LHDC_LL             = 0x29000000u,
    RT_AUDIO_FORMAT_APTX_TWSP           = 0x2A000000u,
    RT_AUDIO_FORMAT_MAIN_MASK           = 0xFF000000u,
    RT_AUDIO_FORMAT_SUB_MASK            = 0x00FFFFFFu,

    /* Subformats */
    RT_AUDIO_FORMAT_PCM_SUB_16_BIT        = 0x1u,
    RT_AUDIO_FORMAT_PCM_SUB_8_BIT         = 0x2u,
    RT_AUDIO_FORMAT_PCM_SUB_32_BIT        = 0x3u,
    RT_AUDIO_FORMAT_PCM_SUB_8_24_BIT      = 0x4u,
    RT_AUDIO_FORMAT_PCM_SUB_FLOAT         = 0x5u,
    RT_AUDIO_FORMAT_PCM_SUB_24_BIT_PACKED = 0x6u,

    RT_AUDIO_FORMAT_MP3_SUB_NONE          = 0x0u,

    RT_AUDIO_FORMAT_AMR_SUB_NONE          = 0x0u,

    RT_AUDIO_FORMAT_AAC_SUB_MAIN          = 0x1u,
    RT_AUDIO_FORMAT_AAC_SUB_LC            = 0x2u,
    RT_AUDIO_FORMAT_AAC_SUB_SSR           = 0x4u,
    RT_AUDIO_FORMAT_AAC_SUB_LTP           = 0x8u,
    RT_AUDIO_FORMAT_AAC_SUB_HE_V1         = 0x10u,
    RT_AUDIO_FORMAT_AAC_SUB_SCALABLE      = 0x20u,
    RT_AUDIO_FORMAT_AAC_SUB_ERLC          = 0x40u,
    RT_AUDIO_FORMAT_AAC_SUB_LD            = 0x80u,
    RT_AUDIO_FORMAT_AAC_SUB_HE_V2         = 0x100u,
    RT_AUDIO_FORMAT_AAC_SUB_ELD           = 0x200u,
    RT_AUDIO_FORMAT_AAC_SUB_XHE           = 0x300u,

    RT_AUDIO_FORMAT_VORBIS_SUB_NONE       = 0x0u,

    RT_AUDIO_FORMAT_E_AC3_SUB_JOC         = 0x1u,

    RT_AUDIO_FORMAT_MAT_SUB_1_0           = 0x1u,
    RT_AUDIO_FORMAT_MAT_SUB_2_0           = 0x2u,
    RT_AUDIO_FORMAT_MAT_SUB_2_1           = 0x3u,

    /* Aliases */
    RT_AUDIO_FORMAT_PCM_16_BIT            = 0x1u,        // (PCM | PCM_SUB_16_BIT)
    RT_AUDIO_FORMAT_PCM_8_BIT             = 0x2u,        // (PCM | PCM_SUB_8_BIT)
    RT_AUDIO_FORMAT_PCM_32_BIT            = 0x3u,        // (PCM | PCM_SUB_32_BIT)
    RT_AUDIO_FORMAT_PCM_8_24_BIT          = 0x4u,        // (PCM | PCM_SUB_8_24_BIT)
    RT_AUDIO_FORMAT_PCM_FLOAT             = 0x5u,        // (PCM | PCM_SUB_FLOAT)
    RT_AUDIO_FORMAT_PCM_24_BIT_PACKED     = 0x6u,        // (PCM | PCM_SUB_24_BIT_PACKED)
    RT_AUDIO_FORMAT_AAC_MAIN              = 0x4000001u,  // (AAC | AAC_SUB_MAIN)
    RT_AUDIO_FORMAT_AAC_LC                = 0x4000002u,  // (AAC | AAC_SUB_LC)
    RT_AUDIO_FORMAT_AAC_SSR               = 0x4000004u,  // (AAC | AAC_SUB_SSR)
    RT_AUDIO_FORMAT_AAC_LTP               = 0x4000008u,  // (AAC | AAC_SUB_LTP)
    RT_AUDIO_FORMAT_AAC_HE_V1             = 0x4000010u,  // (AAC | AAC_SUB_HE_V1)
    RT_AUDIO_FORMAT_AAC_SCALABLE          = 0x4000020u,  // (AAC | AAC_SUB_SCALABLE)
    RT_AUDIO_FORMAT_AAC_ERLC              = 0x4000040u,  // (AAC | AAC_SUB_ERLC)
    RT_AUDIO_FORMAT_AAC_LD                = 0x4000080u,  // (AAC | AAC_SUB_LD)
    RT_AUDIO_FORMAT_AAC_HE_V2             = 0x4000100u,  // (AAC | AAC_SUB_HE_V2)
    RT_AUDIO_FORMAT_AAC_ELD               = 0x4000200u,  // (AAC | AAC_SUB_ELD)
    RT_AUDIO_FORMAT_AAC_XHE               = 0x4000300u,  // (AAC | AAC_SUB_XHE)
    RT_AUDIO_FORMAT_AAC_ADTS_MAIN         = 0x1e000001u, // (AAC_ADTS | AAC_SUB_MAIN)
    RT_AUDIO_FORMAT_AAC_ADTS_LC           = 0x1e000002u, // (AAC_ADTS | AAC_SUB_LC)
    RT_AUDIO_FORMAT_AAC_ADTS_SSR          = 0x1e000004u, // (AAC_ADTS | AAC_SUB_SSR)
    RT_AUDIO_FORMAT_AAC_ADTS_LTP          = 0x1e000008u, // (AAC_ADTS | AAC_SUB_LTP)
    RT_AUDIO_FORMAT_AAC_ADTS_HE_V1        = 0x1e000010u, // (AAC_ADTS | AAC_SUB_HE_V1)
    RT_AUDIO_FORMAT_AAC_ADTS_SCALABLE     = 0x1e000020u, // (AAC_ADTS | AAC_SUB_SCALABLE)
    RT_AUDIO_FORMAT_AAC_ADTS_ERLC         = 0x1e000040u, // (AAC_ADTS | AAC_SUB_ERLC)
    RT_AUDIO_FORMAT_AAC_ADTS_LD           = 0x1e000080u, // (AAC_ADTS | AAC_SUB_LD)
    RT_AUDIO_FORMAT_AAC_ADTS_HE_V2        = 0x1e000100u, // (AAC_ADTS | AAC_SUB_HE_V2)
    RT_AUDIO_FORMAT_AAC_ADTS_ELD          = 0x1e000200u, // (AAC_ADTS | AAC_SUB_ELD)
    RT_AUDIO_FORMAT_AAC_ADTS_XHE          = 0x1e000300u, // (AAC_ADTS | AAC_SUB_XHE)
    RT_AUDIO_FORMAT_AAC_LATM_LC           = 0x25000002u, // (AAC_LATM | AAC_SUB_LC)
    RT_AUDIO_FORMAT_AAC_LATM_HE_V1        = 0x25000010u, // (AAC_LATM | AAC_SUB_HE_V1)
    RT_AUDIO_FORMAT_AAC_LATM_HE_V2        = 0x25000100u, // (AAC_LATM | AAC_SUB_HE_V2)
    RT_AUDIO_FORMAT_E_AC3_JOC             = 0xA000001u,  // (E_AC3 | E_AC3_SUB_JOC)
    RT_AUDIO_FORMAT_MAT_1_0               = 0x24000001u, // (MAT | MAT_SUB_1_0)
    RT_AUDIO_FORMAT_MAT_2_0               = 0x24000002u, // (MAT | MAT_SUB_2_0)
    RT_AUDIO_FORMAT_MAT_2_1               = 0x24000003u, // (MAT | MAT_SUB_2_1)
} RTAudioFormat;

typedef enum {
    RT_AUDIO_OUTPUT_FLAG_NONE             = 0x0,
    RT_AUDIO_OUTPUT_FLAG_DIRECT           = 0x1,
    RT_AUDIO_OUTPUT_FLAG_PRIMARY          = 0x2,
    RT_AUDIO_OUTPUT_FLAG_FAST             = 0x4,
    RT_AUDIO_OUTPUT_FLAG_DEEP_BUFFER      = 0x8,
    RT_AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD = 0x10,
    RT_AUDIO_OUTPUT_FLAG_NON_BLOCKING     = 0x20,
    RT_AUDIO_OUTPUT_FLAG_HW_AV_SYNC       = 0x40,
    RT_AUDIO_OUTPUT_FLAG_TTS              = 0x80,
    RT_AUDIO_OUTPUT_FLAG_RAW              = 0x100,
    RT_AUDIO_OUTPUT_FLAG_SYNC             = 0x200,
    RT_AUDIO_OUTPUT_FLAG_IEC958_NONAUDIO  = 0x400,
    RT_AUDIO_OUTPUT_FLAG_DIRECT_PCM       = 0x2000,
    RT_AUDIO_OUTPUT_FLAG_MMAP_NOIRQ       = 0x4000,
    RT_AUDIO_OUTPUT_FLAG_VOIP_RX          = 0x8000,
    RT_AUDIO_OUTPUT_FLAG_INCALL_MUSIC     = 0x10000,
} RTAudioOutputFlags;

//Determines the current algorithm used for stretching
typedef enum {
    RT_AUDIO_TIMESTRETCH_STRETCH_DEFAULT            = 0,
    RT_AUDIO_TIMESTRETCH_STRETCH_SPEECH             = 1,
    //TODO: add more stretch modes/algorithms
} RTAudioTimestretchStretchMode;

//Determines behavior of Timestretch if current algorithm can't perform
//with current parameters.
// FALLBACK_CUT_REPEAT: (internal only) for speed <1.0 will truncate frames
//    for speed > 1.0 will repeat frames
// FALLBACK_MUTE: will set all processed frames to zero
// FALLBACK_FAIL:  will stop program execution and log a fatal error
typedef enum {
    RT_AUDIO_TIMESTRETCH_FALLBACK_CUT_REPEAT     = -1,
    RT_AUDIO_TIMESTRETCH_FALLBACK_DEFAULT        = 0,
    RT_AUDIO_TIMESTRETCH_FALLBACK_MUTE           = 1,
    RT_AUDIO_TIMESTRETCH_FALLBACK_FAIL           = 2,
} RTAudioTimestretchFallbackMode;

typedef struct RT_AUDIO_PLAYBACK_RATE {
    float                           mSpeed;
    float                           mPitch;
    RTAudioTimestretchStretchMode   mStretchMode;
    RTAudioTimestretchFallbackMode  mFallbackMode;
} RTAudioPlaybackRate;

typedef struct RT_AUDIO_SINK_PARAM {
    int32_t             mode;
    int32_t             sampleRate;
    int32_t             channels;
    uint32_t            channelMask;
    RTAudioFormat       format;
    RTAudioOutputFlags  flags;
    void               *cookie;
} RTAudioSinkParam;

class RTAudioSinkCBInterface {
 public:
    virtual int32_t open(void *param) = 0;
    virtual int32_t start() = 0;
    virtual int32_t pause() = 0;
    virtual int32_t stop() = 0;
    virtual int32_t flush() = 0;
    virtual int32_t close() = 0;

    virtual int32_t write(const void *buffer, int32_t size) = 0;
    virtual uint32_t latency() = 0;
    virtual int32_t frameSize() = 0;
    virtual int32_t getPlaybackRate(RTAudioPlaybackRate *param) = 0;
    virtual int32_t setPlaybackRate(RTAudioPlaybackRate param) = 0;
    virtual int64_t getPlayedOutDurationUs() = 0;

    virtual ~RTAudioSinkCBInterface() {}
};

#endif  // ROCKIT_HIDL_V1_0_UTILS_RTAUDIOSINKCBINTERFACE_H_

