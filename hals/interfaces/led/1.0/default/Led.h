// FIXME: your file license if you have one

#pragma once

#include <android/hardware/rpi/led/1.0/ILed.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android::hardware::rpi::led::implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct Led : public V1_0::ILed {
    // Methods from ::android::hardware::rpi::led::V1_0::ILed follow.
    Return<int8_t> init(const hidl_string& chipname, uint8_t line, const hidl_string& id) override;
    Return<int8_t> requestOuput(const hidl_string& id) override;
    Return<void> setOuput(const hidl_string& id, uint8_t val) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" ILed* HIDL_FETCH_ILed(const char* name);

}  // namespace android::hardware::rpi::led::implementation
