/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "nnapi_implementation.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <errno.h>
#include <android/log.h>

#include <sys/system_properties.h>

#define TAG "nnapi_implementation.a"
#define NNAPI_LOG(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)

namespace {

int32_t GetAndroidSdkVersion() {
  const char* sdkProp = "ro.build.version.sdk";
  char sdkVersion[PROP_VALUE_MAX];
  int length = __system_property_get(sdkProp, sdkVersion);
  if (length != 0) {
    int32_t result = 0;
    for (int i = 0; i < length; ++i) {
      int digit = sdkVersion[i] - '0';
      if (digit < 0 || digit > 9) {
        // Non-numeric SDK version, assume it's higher than expected;
        return 0xffff;
      }
      result = result * 10 + digit;
    }
    // TODO(levp): remove once SDK gets updated to 29th level
    // Upgrade SDK version for pre-release Q to be able to test functionality
    // available from SDK level 29.
    if (result == 28) {
      char versionCodename[PROP_VALUE_MAX];
      const char* versionCodenameProp = "ro.build.version.codename";
      length = __system_property_get(versionCodenameProp, versionCodename);
      if (length != 0) {
        if (versionCodename[0] == 'Q') {
          return 29;
        }
      }
    }
    return result;
  }
  return 0;
}

void* LoadFunction(void* handle, const char* name, bool optional) {
  if (handle == nullptr) {
    return nullptr;
  }
  void* fn = dlsym(handle, name);
  if (fn == nullptr && !optional) {
    NNAPI_LOG("nnapi error: unable to open function %s, %s", name, dlerror());
  }
  return fn;
}

#define LOAD_FUNCTION(handle, name)         \
  nnapi.name = reinterpret_cast<name##_fn>( \
      LoadFunction(handle, #name, /*optional*/ false));

#define LOAD_FUNCTION_OPTIONAL(handle, name) \
  nnapi.name = reinterpret_cast<name##_fn>(  \
      LoadFunction(handle, #name, /*optional*/ true));

#define LOAD_FUNCTION_RENAME(handle, name, symbol) \
  nnapi.name = reinterpret_cast<name##_fn>(        \
      LoadFunction(handle, symbol, /*optional*/ false));

const NnApi LoadNnApi() {
  NnApi nnapi = {};
  nnapi.android_sdk_version = 0;

  nnapi.android_sdk_version = GetAndroidSdkVersion();
  if (nnapi.android_sdk_version < 27) {
    NNAPI_LOG("nnapi error: requires android sdk version to be at least %d",
              27);
    nnapi.nnapi_exists = false;
    return nnapi;
  }

  void* librknnhal_bridge = nullptr;
  // TODO(b/123243014): change RTLD_LOCAL? Assumes there can be multiple
  // instances of nn api RT
  librknnhal_bridge = dlopen("librknnhal_bridge.so", RTLD_LAZY | RTLD_LOCAL);
  if (librknnhal_bridge == nullptr) {
    NNAPI_LOG("nnapi error: unable to open library %s, %s", "librknnhal_bridge.so", strerror(errno));
  }

  nnapi.nnapi_exists = librknnhal_bridge != nullptr;

  // API 27 (NN 1.0) methods.
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_client_create);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_find_devices);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_init);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_init2);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_destroy);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_query);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_inputs_set);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_run);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_outputs_get);
  LOAD_FUNCTION(librknnhal_bridge, ARKNN_outputs_release);

  /* Not sure this func exist.
   * LOAD_FUNCTION_OPTIONAL(
      librknnhal_bridge,
      ANeuralNetworksModel_setOperandSymmPerChannelQuantParams);*/

  // ASharedMemory_create has different implementations in Android depending on
  // the partition. Generally it can be loaded from libandroid.so but in vendor
  // partition (e.g. if a HAL wants to use NNAPI) it is only accessible through
  // libcutils.
  void* libandroid = nullptr;
  libandroid = dlopen("libandroid.so", RTLD_LAZY | RTLD_LOCAL);
  if (libandroid != nullptr) {
    LOAD_FUNCTION(libandroid, ASharedMemory_create);
  } else {
    void* cutils_handle = dlopen("libcutils.so", RTLD_LAZY | RTLD_LOCAL);
    if (cutils_handle != nullptr) {
      LOAD_FUNCTION_RENAME(cutils_handle, ASharedMemory_create,
                           "ashmem_create_region");
    } else {
      NNAPI_LOG("nnapi error: unable to open neither libraries %s and %s",
                "libandroid.so", "libcutils.so");
    }
  }
  return nnapi;
}

}  // namespace

const NnApi* NnApiImplementation() {
  static const NnApi nnapi = LoadNnApi();
  return &nnapi;
}
