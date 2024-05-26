#define LOG_TAG "LedService"

#include <iostream>
#include <android/hardware/rpi/led/1.0/ILed.h>
#include <hidl/HidlTransportSupport.h>
#include <hidl/LegacySupport.h>
#include <utils/Log.h>
#include "Led.h"

using android::hardware::rpi::led::V1_0::ILed;
using android::hardware::rpi::led::implementation::Led;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

void logd(std::string msg) {
    std::cout << msg << std::endl;
    ALOGD("%s", msg.c_str());
}

void loge(std::string msg) {
    std::cout << msg << std::endl;
    ALOGE("%s", msg.c_str());
}

int main(int /* argc */, char** /* argv */) {
    configureRpcThreadpool(1, true /*callerWillJoin*/);

    android::sp<ILed> led = new Led();
    if (led != nullptr) {
        if (led->registerAsService() != ::android::OK) {
            loge("Failed to register ILed service");
            return -1;
        }
    } else {
        loge("Failed to get ILed instance");
        return -1;
    }

    logd("ILed service starts to join service pool");
    joinRpcThreadpool();
    return 1;  // joinRpcThreadpool shouldn't exit
}
