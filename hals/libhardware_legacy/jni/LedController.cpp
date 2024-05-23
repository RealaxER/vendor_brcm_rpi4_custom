#include <jni.h>
#include "LedController.h"
#include <hardware_legacy/leds.h>
#include <string>
#include <memory>
#include <stdexcept>

#define LED_JNI_OK 1
#define LED_JNI_ERR -1

class LedController_t {
private:
    std::string name;
    unsigned char val;
    std::unique_ptr<led_t> led;

public:
    LedController_t(const std::string& chipname, unsigned int line_num, const std::string& name) : name(name) {
        led = std::unique_ptr<led_t>(leds_init(chipname.c_str(), line_num, name.c_str()));
        if (!led) {
            throw std::runtime_error("Failed to initialize LED");
        }
    }

    int requestOutput() {
        if (leds_request_output(this->name.c_str(), this->val)) {
            return LED_JNI_OK;
        }
        return LED_JNI_ERR;
    }

    int setOutput(unsigned char val) {
        if (leds_set_output(this->name.c_str(), val)) {
            this->val = val;
            return LED_JNI_OK;
        }
        return LED_JNI_ERR;
    }

    int setBlinkOutput(unsigned int time) {
        if (leds_set_tim_output(this->name.c_str(), time)) {
            return LED_JNI_OK;
        }
        return LED_JNI_ERR;
    }
    int stopBlink(void) {
        if (leds_stop_blink(this->name.c_str())) {
            return LED_JNI_OK;
        }
        return LED_JNI_ERR;
    }
};

extern "C" {

JNIEXPORT void JNICALL Java_LedController_init(JNIEnv *env, jobject obj, jstring chipname, jint lineNum, jstring name) {
    const char *c_chipname = env->GetStringUTFChars(chipname, nullptr);
    const char *c_name = env->GetStringUTFChars(name, nullptr);

    try {
        LedController_t *ledJni = new LedController_t(c_chipname, lineNum, c_name);

        jclass clazz = env->GetObjectClass(obj);
        jfieldID nativeHandleField = env->GetFieldID(clazz, "nativeHandle", "J");
        env->SetLongField(obj, nativeHandleField, reinterpret_cast<jlong>(ledJni));
    } catch (std::exception& e) {
        jclass exceptionClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exceptionClass, e.what());
    }

    env->ReleaseStringUTFChars(chipname, c_chipname);
    env->ReleaseStringUTFChars(name, c_name);
}

JNIEXPORT jint JNICALL Java_LedController_requestOutput(JNIEnv *env, jobject obj) {
    jclass clazz = env->GetObjectClass(obj);
    jfieldID nativeHandleField = env->GetFieldID(clazz, "nativeHandle", "J");
    LedController_t *ledJni = reinterpret_cast<LedController_t*>(env->GetLongField(obj, nativeHandleField));

    if (ledJni == nullptr) {
        return LED_JNI_ERR;
    }

    return ledJni->requestOutput();
}

JNIEXPORT jint JNICALL Java_LedController_setOutput(JNIEnv *env, jobject obj, jbyte val) {
    jclass clazz = env->GetObjectClass(obj);
    jfieldID nativeHandleField = env->GetFieldID(clazz, "nativeHandle", "J");
    LedController_t *ledJni = reinterpret_cast<LedController_t*>(env->GetLongField(obj, nativeHandleField));

    if (ledJni == nullptr) {
        return LED_JNI_ERR;
    }

    return ledJni->setOutput(static_cast<unsigned char>(val));
}


JNIEXPORT jint JNICALL Java_LedController_setBlinkOutput(JNIEnv * env, jobject obj, jbyte time) {
    jclass clazz = env->GetObjectClass(obj);
    jfieldID nativeHandleField = env->GetFieldID(clazz, "nativeHandle", "J");
    LedController_t *ledJni = reinterpret_cast<LedController_t*>(env->GetLongField(obj, nativeHandleField));

    if (ledJni == nullptr) {
        return LED_JNI_ERR;
    }

    return ledJni->setBlinkOutput(static_cast<unsigned char>(time));
}


JNIEXPORT jint JNICALL Java_LedController_stopBlink(JNIEnv * env, jobject obj) {
    jclass clazz = env->GetObjectClass(obj);
    jfieldID nativeHandleField = env->GetFieldID(clazz, "nativeHandle", "J");
    LedController_t *ledJni = reinterpret_cast<LedController_t*>(env->GetLongField(obj, nativeHandleField));

    if (ledJni == nullptr) {
        return LED_JNI_ERR;
    }
    return ledJni->stopBlink();
}

}
