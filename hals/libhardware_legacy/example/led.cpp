#include <hardware_legacy/leds.h>
#include <string>
#include <iostream>
#include <memory>
#include <unistd.h>

#define LED_JNI_OK 1
#define LED_JNI_ERR -1

class LedJni_t {
private:
    std::string name;
    unsigned char val;
public:
    LedJni_t(std::string chipname, unsigned int line_num, std::string name) : name(name) {
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
    
    int close() {
        if (led_remove_by_id(this->name.c_str())) {
            return LED_JNI_OK;
        }
        return LED_JNI_ERR;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <value>" << std::endl;
        return LED_JNI_ERR;
    }
    int time = std::stoi(argv[1]);

    try {
        LedJni_t led("gpiochip0", 21, "led_21");
        
        int result = led.requestOutput();
        if (result == LED_JNI_OK) {
            std::cout << "Requested output successfully" << std::endl;
        } else {
            std::cerr << "Failed to request output" << std::endl;
        }
        
        while(1) {
            result = led.setOutput(1);
            sleep(time);
            result = led.setOutput(0);
            sleep(time);
        }
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
