#include "jni.h"
#include <nativehelper/JNIHelp.h>
#include "android_runtime/AndroidRuntime.h"
#include <stdexcept>
#include <utils/misc.h>
#include <utils/Log.h>
#include <unordered_map>
#include <hardware_legacy/leds.h>

#define LED_JNI_OK 1
#define LED_JNI_ERR -1

namespace rpi { 
    class LedController_t {
    private:
        std::string name;
        unsigned char val;
    public:
        LedController_t(const std::string& chipname, unsigned int line_num, const std::string& name) : name(name) {
            leds_init(chipname.c_str(), line_num, name.c_str());
        }
        
        int requestOutput() {
            if (leds_request_output(this->name.c_str(), this->val)) {
                return LED_JNI_OK;
            }
            return LED_JNI_ERR;
        }

        int setOutput(unsigned char value) {
            if (leds_set_output(this->name.c_str(), value)) {
                this->val = value;
                return LED_JNI_OK;
            }
            return LED_JNI_ERR;
        }

        int setBlinkLed(unsigned int delay) {
            if (leds_set_tim_output(this->name.c_str(), delay)) {
                return LED_JNI_OK;
            }
            return LED_JNI_ERR;
        }

        int setStopBlink(void) {
            if (leds_stop_blink(this->name.c_str())) {
                return LED_JNI_OK;
            }
            return LED_JNI_ERR;
        }
        
        int close(void) {
            if (led_remove_by_id(this->name.c_str())) {
                return LED_JNI_OK;
            }
            return LED_JNI_ERR;
        }
    };

    static std::unordered_map<long, LedController_t*>& getLedInstances() {
        static std::unordered_map<long, LedController_t*> ledInstances;
        return ledInstances;
    }

    static void jni_LedController_init(JNIEnv *env, jobject obj, jstring chipname, jint line_num, jstring ledname) {
        const char *c_chipname = env->GetStringUTFChars(chipname, nullptr);
        const char *c_ledname = env->GetStringUTFChars(ledname, nullptr);

        LedController_t *ledInstance = new LedController_t(c_chipname, static_cast<unsigned int>(line_num), c_ledname);

        jlong address = reinterpret_cast<jlong>(ledInstance);
        getLedInstances()[address] = ledInstance;

        env->ReleaseStringUTFChars(chipname, c_chipname);
        env->ReleaseStringUTFChars(ledname, c_ledname);
    }

    static jint jni_LedController_requestOutput(JNIEnv *env, jobject obj) {
        try {
            jlong address = reinterpret_cast<jlong>(obj);
            LedController_t *ledInstance = getLedInstances()[address];
            if (!ledInstance) {
                throw std::runtime_error("LED does not exist");
            }
            return ledInstance->requestOutput();
        } catch (const std::runtime_error &e) {
            ALOGD("JNI LED request output error");
            return LED_JNI_ERR;
        }
    }

    static jint jni_LedController_setOutput(JNIEnv *env, jobject obj, jbyte value) {
        try {
            jlong address = reinterpret_cast<jlong>(obj);
            LedController_t *ledInstance = getLedInstances()[address];
            if (!ledInstance) {
                throw std::runtime_error("LED does not exist");
            }
            return ledInstance->setOutput(static_cast<unsigned char>(value));
        } catch (const std::runtime_error &e) {
            ALOGD("JNI LED set output error");
            return LED_JNI_ERR;
        }
    }

    static const JNINativeMethod method_table[] = {
        { "led_native_init", "(Ljava/lang/String;ILjava/lang/String;)V", (void*)jni_LedController_init },
        { "led_native_requestOutput", "()I", (void*)jni_LedController_requestOutput },
        { "led_native_setOutput", "(B)I", (void*)jni_LedController_setOutput },
    };

    int register_android_rpi_gpio_LedController(JNIEnv *env) {
        ALOGD("JNI: register_android_rpi_gpio_LedController\n");
        return jniRegisterNativeMethods(
                env, 
                "com/android/rpi/gpio/LedController",
                method_table, 
                NELEM(method_table)
            );
    }
};
