#### 迁移rknn_api到rknn_hal的方法:
1. 拷贝3rdparty/nnapi_implementation到App的jni目录；
2. CMakeList中，移除原有的librknn_api.so的链接，添加nnapi_implementation/include到include搜索路径，并将nnapi_implementation.a链接到jni中；
3. 修改jni代码中，依赖"rknn_api.h"改为"nnapi_implementation.h"
4. jni中声明以下变量:
```c/c++
const NnApi* nnapi;
ARKNNHAL *hal;
```
5. jni中，在调用任何rknn_api的函数之前，添加如下代码:
```c/c++
nnapi = NnApiImplementation();
if (nnapi && nnapi->nnapi_exists) {
    nnapi->ARKNN_client_create(&hal);
    if (!hal) {
        LOGE("Failed to create RKNN HAL Client!");
        return;
    }
}
```

#### 示例补丁
```rknnapi_to_rknnhal.diff```
