#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <hardware/led_conventional.h>


int main(void) {
    int err = 0;
    struct led_module_t *module = NULL;
    struct led_device_t *device = NULL;

    printf("In test hal conventional led\n");

    err = hw_get_module(
        LED_HARDWARE_MODULE_ID,
        (struct hw_module_t const**)&module
    );

    if(err !=0){
        printf("Failed to get module\n");
        return -1;
    }else {
        printf("Got module\n");
        if(module->common.methods->open(
            (struct hw_module_t*) module,
            LED_HARDWARE_MODULE_ID,
            (struct hw_device_t **)&device
        )!=0){
            printf("Failed to get device\n");
            return -1;
        }else {
            printf("Opened device\n");
        }
    }

    printf("Handle led\n");
    const char * led_id = "led 21";

    led_t *led  = device->init("gpiochip0", 21, led_id);
    if(led == NULL){
        printf("Failed to handle led\n");
        return -1;
    }

    err = device->request_output(led_id , 0);

    err = device->set_output(led_id, 1);
    sleep(3);

    for(int i =0; i< 20; i++){
        err = device->set_output(led_id, 0);
        sleep(1);
        err = device->set_output(led_id, 1);
        sleep(1);
    }
    err = device->cleanup();
    printf("Cleaned up\n");
    return 0;
}