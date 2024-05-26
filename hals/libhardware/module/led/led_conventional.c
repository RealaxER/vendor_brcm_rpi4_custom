#define LOG_TAG "Led_conventional"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "hardware/led_conventional.h"

pthread_mutex_t led_list_mutex = PTHREAD_MUTEX_INITIALIZER;

led_t *led_list_head = NULL;

struct led_device_t * led_dev;


static led_t* led_init(const char *chipname, unsigned int line_num, const char *led_id);
static led_t* led_find_by_id(const char *led_id);
static size_t led_cleanup();
static size_t led_remove_by_id(const char *led_id);
static size_t led_request_output(const char *id, unsigned char default_val);
static size_t led_set_output(const char *id, unsigned int state);
static size_t led_set_tim_output(const char *id, unsigned int frequency);
static size_t led_stop_blink(const char *id);

static int led_open(const struct hw_module_t *module, const char *name, struct hw_device_t **device);
static int led_close(struct hw_device_t* device);

static struct hw_module_methods_t led_module_methods = {
    .open = led_open,
};

struct led_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = LED_HARDWARE_MODULE_ID,
        .name = LED_HARDWARE_MODULE_ID,
        .author = "bdHien",
        .methods = &led_module_methods,
    },  
};

static void __attribute__ ((constructor)) led_loaded() {
    ALOGD("Conventional LED HAL Module: Loaded");
}

static void __attribute__ ((destructor )) led_unloaded() {
    ALOGD("Conventional LED HAL Module: Unloaded");
}

static int led_open( 
    const struct hw_module_t *module, 
    const char *id, 
    struct hw_device_t **device
) {
    ALOGD("Conventional LED HAL Module: Open");
    led_dev = malloc(sizeof(struct led_device_t));
    if(led_dev == NULL) {
        ALOGE("Conventional LED HAL Module: malloc failed");
        return -ENOMEM;
    }

    led_dev->common.tag = HARDWARE_DEVICE_TAG;
    led_dev->common.version = 1;
    led_dev->common.module = (struct hw_module_t *)module;
    led_dev->common.close = led_close;
    led_dev->init = led_init;
    led_dev->find_by_id = led_find_by_id;
    led_dev->cleanup = led_cleanup;
    led_dev->remove_by_id = led_remove_by_id;
    led_dev->request_output = led_request_output;
    led_dev->set_output = led_set_output;
    led_dev->set_tim_output = led_set_tim_output;
    led_dev->stop_blink = led_stop_blink;

    *device = (struct hw_device_t *)led_dev;
    ALOGD("Conventional LED HAL Module: Device %s is connected\n", id);

    return 0;
}

static int led_close(struct hw_device_t *device) {
    if(device) {
        free(device);
        ALOGD("Conventional LED HAL Module: free device");
    }
    return 0;
}

static led_t* led_init(const char *chipname, unsigned int line_num, const char *led_id) {

    led_t *new_led = (led_t *)malloc(sizeof(led_t));
    if (!new_led) {
        LED_LOG("LED malloc failed");
        return NULL;
    }

    new_led->chip = gpiod_chip_open_by_name(chipname);
    if (!new_led->chip) {
        LED_LOG("LED open chip failed");
        free(new_led);
        return NULL;
    }

    new_led->line = gpiod_chip_get_line(new_led->chip, line_num);
    if (!new_led->line) {
        LED_LOG("LED get line failed");
        gpiod_chip_close(new_led->chip);
        free(new_led);
        return NULL;
    }

    strncpy(new_led->id, led_id, sizeof(new_led->id) - 1);
    new_led->id[sizeof(new_led->id) - 1] = '\0';

    pthread_mutex_lock(&led_list_mutex);

    new_led->next = led_list_head;
    led_list_head = new_led;

    pthread_mutex_unlock(&led_list_mutex);

    return new_led;
}

static size_t led_cleanup() {
    pthread_mutex_lock(&led_list_mutex);

    led_t *current = led_list_head;
    while (current != NULL) {
        led_t *temp = current;
        current = current->next;

        gpiod_chip_close(temp->chip);
        free(temp);
    }

    led_list_head = NULL;

    pthread_mutex_unlock(&led_list_mutex);

    return LED_OK;
}

static led_t* led_find_by_id(const char *led_id) {
    pthread_mutex_lock(&led_list_mutex);

    led_t *current = led_list_head;
    while (current != NULL) {
        if (strcmp(current->id, led_id) == 0) {
            pthread_mutex_unlock(&led_list_mutex);
            return current;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&led_list_mutex);
    return NULL;
}

static size_t led_remove_by_id(const char *led_id) {
    pthread_mutex_lock(&led_list_mutex);

    led_t *current = led_list_head;
    led_t *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->id, led_id) == 0) {
            if (previous == NULL) {
                led_list_head = current->next;
            } else {
                previous->next = current->next;
            }

            gpiod_chip_close(current->chip);
            free(current);

            pthread_mutex_unlock(&led_list_mutex);
            return LED_OK; 
        }
        previous = current;
        current = current->next;
    }

    pthread_mutex_unlock(&led_list_mutex);
    return LED_ERR;
}


static size_t led_request_output(const char *id, unsigned char default_val) {
    led_t *led = led_find_by_id(id);
    if (led) {
        if (gpiod_line_request_output(led->line, id, default_val) == 0) {
            led->val = default_val;
            return LED_OK;
        }
    }
    return LED_ERR;
}

static size_t led_set_output(const char *id, unsigned int state) {
    led_t *led = led_find_by_id(id);
    if (led) {
        if ( gpiod_line_set_value(led->line, state) == 0) {
            led->val = state;
            return LED_OK;
        }
    }
    return LED_ERR;
}

static void* blink_led(void* args) {
    led_t *led = (led_t*) args;

    while (led->should_blink) {
        gpiod_line_set_value(led->line, 1);
        usleep(led->delay);
        gpiod_line_set_value(led->line, 0);
        usleep(led->delay);
    }
    return NULL;
}

static size_t led_set_tim_output(const char *id, unsigned int frequency) {

    led_t *led = led_find_by_id(id);
    if (led) {
        led->should_blink = 1;
        led->delay = frequency;
        if (pthread_create(&led->blink_thread, NULL, blink_led, (void*) led) != 0) {
            LED_LOG("pthread_create failed");
            return LED_ERR;
        }
        return LED_OK;
    }
    return LED_ERR;
}

static size_t led_stop_blink(const char *id) {
    led_t *led = led_find_by_id(id);
    if (led) {
        led->should_blink = 0;
        pthread_join(led->blink_thread, NULL);
        return LED_OK;
    }
    return LED_ERR;
}
