#include "RockchipNeuralnetworksBuilder.h"

using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hardware::hidl_memory;

using namespace std;
using namespace rockchip::hardware::neuralnetworks::V1_0;
using namespace android;

using ::android::hardware::Return;
using ::android::hardware::Void;

#define CHECK() \
    ALOGE("%s", __func__)

static RKNNQueryCmd to_rknn_hal(rknn_query_cmd cmd) {
    switch (cmd) {
        case RKNN_QUERY_IN_OUT_NUM:
            return RKNNQueryCmd::RKNN_QUERY_IN_OUT_NUM;
        case RKNN_QUERY_INPUT_ATTR:
            return RKNNQueryCmd::RKNN_QUERY_INPUT_ATTR;
        case RKNN_QUERY_OUTPUT_ATTR:
            return RKNNQueryCmd::RKNN_QUERY_OUTPUT_ATTR;
        case RKNN_QUERY_PERF_DETAIL:
            return RKNNQueryCmd::RKNN_QUERY_PERF_DETAIL;
        case RKNN_QUERY_PERF_RUN:
            return RKNNQueryCmd::RKNN_QUERY_PERF_RUN;
        case RKNN_QUERY_SDK_VERSION:
            return RKNNQueryCmd::RKNN_QUERY_SDK_VERSION;
        default:
            return RKNNQueryCmd::RKNN_QUERY_CMD_MAX;
    }
}

namespace rockchip {
namespace nn {

RockchipNeuralnetworksBuilder::RockchipNeuralnetworksBuilder() {
    _kRKNNInterface = IRKNeuralnetworks::getService();
    if (_kRKNNInterface == NULL) {
        ALOGE("Failed to getService: IRKNeuralnetworks[vendor.rknn-1-0]!");
    }
    _kAllocInterface = IAllocator::getService("ashmem");
    if (_kAllocInterface == NULL) {
        ALOGE("Failed to getService: IAllocator[android.ashmem-1-0]!");
    }

    ALOGE("Successfully initial.");
}

int RockchipNeuralnetworksBuilder::rknn_find_devices(rknn_devices_id* pdevs) {
    CHECK();
    Return<void> ret = _kRKNNInterface->rknnFindDevices([&](ErrorStatus status, RKNNDeviceID devices) {
        if ((int)status == RKNN_SUCC) {
            
        } else {

        }
    });
    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_init(rknn_context* context, void *pData, uint32_t size, uint32_t flag) {
    CHECK();
    _kAllocInterface->allocate(size, [&](bool success, const hidl_memory& mem) {
        if (!success) {
            ALOGE("Allocate memory failed!");
        } else {
            const struct RKNNModel model = {
                .name = "mobilenet_v1",
                .path = "/data/mobilenet_v1-tf.rknn",
                .modelData = mem,
            };
            sp<IMemory> memory = mapMemory(mem);
            memory->update();
            memcpy(memory->getPointer(), pData, size);
            memory->commit();

            Return<void> ret =  _kRKNNInterface->rknnInit(model, size, flag, [&](ErrorStatus status, RKNNContext rContext) {
                if ((int)status == RKNN_SUCC) {
                    *context = rContext;
                    ALOGE("Client init Successfully!");
                } else {
                    ALOGE("Client init failed!");
                }
            });
            if (!ret.isOk()) {
                ALOGE("Ret Failed!");
            } else {
                ALOGE("Ret Successfully!");
            }
        }
    });
    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_init2(rknn_context* context, void *pData, uint32_t size, uint32_t flag, rknn_init_extend* extend) {
    CHECK();
    _kAllocInterface->allocate(size, [&](bool success, const hidl_memory& mem) {
        if (!success) {
            ALOGE("Allocate memory failed!");
        } else {
            const struct RKNNModel model = {
                .name = "mobilenet_v1",
                .path = "/data/mobilenet_v1-tf.rknn",
                .modelData = mem,
            };
            sp<IMemory> memory = mapMemory(mem);
            memory->update();
            memcpy(memory->getPointer(), pData, size);
            memory->commit();
            std::string str_device_id(extend->device_id);
            const struct RKNNInitExtend ext {
                .device_id = str_device_id,
            };
            Return<void> ret =  _kRKNNInterface->rknnInit2(model, size, flag, ext, [&](ErrorStatus status, RKNNContext rContext) {
                if ((int)status == RKNN_SUCC) {
                    *context = rContext;
                    ALOGE("Client init Successfully!");
                } else {
                    ALOGE("Client init failed!");
                }
            });
            if (!ret.isOk()) {
                ALOGE("Ret Failed!");
            } else {
                ALOGE("Ret Successfully!");
            }
        }
    });
    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_destroy(rknn_context context) {
    CHECK();
    _kRKNNInterface->rknnDestory(context);
    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_query(rknn_context context, rknn_query_cmd cmd, void* info, uint32_t size) {
    CHECK();
    _kAllocInterface->allocate(size, [&](bool success, const hidl_memory& mem) {
        if (!success) {
            ALOGE("Allocate memory failed!");
        } else {
            _kRKNNInterface->rknnQuery(context, to_rknn_hal(cmd), mem, size);
            sp<IMemory> pMem = mapMemory(mem);
            memcpy(info, pMem->getPointer(), size);
        }
    });
    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_inputs_set(rknn_context context, uint32_t n_inputs, rknn_input inputs[]) {
    CHECK();
    int poolSize = 0;
    for (int i = 0; i < n_inputs; i++) {
        poolSize += inputs[i].size;
    }
    _kAllocInterface->allocate(poolSize, [&](bool success, const hidl_memory& mem) {
        if (!success) {

        } else {
            vector<RKNNInput> input_array;
            sp<IMemory> memory = mapMemory(mem);
            memory->update();
            for (int i = 0; i < n_inputs; i++) {
                // TODO
                // map memory
                memcpy(memory->getPointer(), inputs[i].buf, inputs[i].size);
                const struct DataLocation buf = {
                    .poolIndex = static_cast<uint32_t>(i),
                    .offset = 0,
                    .length = static_cast<uint32_t>(inputs[i].size),
                };
                const struct RKNNInput input0 = {
                    .index = 0,
                    .buf = buf,
                    .pass_through = false,
                    .type = RKNNTensorType::RKNN_TENSOR_UINT8,
                    .fmt = RKNNTensorFormat::RKNN_TENSOR_NHWC,
                };
                input_array.push_back(input0);
            }
            memory->commit();
            const struct Request request = {
                .n_inputs = static_cast<uint32_t>(input_array.size()),
                .inputs = input_array,
                .pool = mem,
            };
            _kRKNNInterface->rknnInputsSet(context, request);
        }
    });
    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_run(rknn_context context, rknn_run_extend* extend) {
    CHECK();
    const struct RKNNRunExtend rExt = {
        .frame_id = extend?extend->frame_id:0,
    };
    _kRKNNInterface->rknnRun(context, rExt);
    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_outputs_get(rknn_context context, uint32_t n_outputs, rknn_output outputs[], rknn_output_extend* extend) {
    CHECK();
    // TODO
    // Calculate result size
    int poolSize = 4004;
    for (int i = 0; i < n_outputs; i++) {
        poolSize += outputs[i].is_prealloc?outputs[i].size:0;
    }
    ALOGE("XXXXXXXXXX %d", poolSize);
    _kAllocInterface->allocate(poolSize, [&](bool success, const hidl_memory& mem) {
        if (!success) {
        } else {
            sp<IMemory> memory = mapMemory(mem);
            vector<RKNNOutput> output_array;
            // TODO
            // map memory
            for (int i = 0; i < n_outputs; i++) {
                const struct DataLocation result = {
                    .poolIndex = 0,
                    .offset = 0,
                    .length = 4004,
                };
                const struct RKNNOutput output0 = {
                    .want_float = true,
                    .is_prealloc = true,
                    .buf = result,
                };
                output_array.push_back(output0);
            }
            const struct RKNNOutputExtend gExt = {
                .frame_id = extend? extend->frame_id:0,
            };
            const struct Response response = {
                .n_outputs = static_cast<uint32_t>(output_array.size()),
                .outputs = output_array,
                .pool = mem,
            };
            _kRKNNInterface->rknnOutputsGet(context, response, gExt);
            void *resultPool = memory->getPointer();
            outputs[0].buf = malloc(4004);
            for (int i = 0; i < n_outputs; i++) {
                memcpy(outputs[i].buf, resultPool, 4004);
            }
        }
    });

    return 0;
}

int RockchipNeuralnetworksBuilder::rknn_outputs_release(rknn_context context, uint32_t n_outputs, rknn_output outputs[]) {
    CHECK();
    //_kRKNNInterface->rknnOutputsRelease(context, );
    return 0;
}

}
}
