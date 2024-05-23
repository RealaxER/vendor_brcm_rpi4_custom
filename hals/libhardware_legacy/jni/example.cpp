#include <hardware_legacy/leds.h>
#include <string>
#include <iostream>
#include <memory>

#define LED_JNI_OK 1
#define LED_JNI_ERR -1

class LedJni_t {
private:
    std::string name;
    unsigned char val;
    std::unique_ptr<led_t> led;

public:
    LedJni_t(std::string chipname, unsigned int line_num, std::string name) : name(name) {
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
};

int main(void) {
    try {
        LedJni_t led("gpiochip0", 21, "led_21");
        
        int result = led.requestOutput();
        if (result == LED_JNI_OK) {
            std::cout << "Requested output successfully" << std::endl;
        } else {
            std::cerr << "Failed to request output" << std::endl;
        }
        
        result = led.setOutput(0);
        if (result == LED_JNI_OK) {
            std::cout << "Set output successfully" << std::endl;
        } else {
            std::cerr << "Failed to set output" << std::endl;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    
    return 0;
}
