// FIXME: your file license if you have one

#include "Led.h"
#include <gpiod.h>
#include <utils/Log.h>


#define LED_LOG(__va_arg__) ALOGE(__va_arg__)   
#define LED_OK 0
#define LED_ERR -1
namespace android::hardware::rpi::led::implementation {


typedef struct led {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    char id[32]; 
    struct led *next;
    unsigned char val;
    pthread_t blink_thread;
    int should_blink;
    unsigned int delay;
} led_t;

led_t *led_list_head = NULL;
pthread_mutex_t led_list_mutex = PTHREAD_MUTEX_INITIALIZER;

static led_t* led_init(const char *chipname, unsigned int line_num, const char *led_id);
static int8_t led_request_output(const char *id, unsigned char default_val);
static int8_t led_set_output(const char *id, unsigned int state);

static led_t* led_find_by_id(const char *led_id);

// static int8_t led_cleanup();
// static int8_t led_remove_by_id(const char *led_id);
// static int8_t led_set_tim_output(const char *id, unsigned int frequency);
// static int8_t led_stop_blink(const char *id);

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

// static int8_t led_cleanup() {
//     pthread_mutex_lock(&led_list_mutex);

//     led_t *current = led_list_head;
//     while (current != NULL) {
//         led_t *temp = current;
//         current = current->next;

//         gpiod_chip_close(temp->chip);
//         free(temp);
//     }

//     led_list_head = NULL;

//     pthread_mutex_unlock(&led_list_mutex);

//     return LED_OK;
// }

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

// static int8_t led_remove_by_id(const char *led_id) {
//     pthread_mutex_lock(&led_list_mutex);

//     led_t *current = led_list_head;
//     led_t *previous = NULL;

//     while (current != NULL) {
//         if (strcmp(current->id, led_id) == 0) {
//             if (previous == NULL) {
//                 led_list_head = current->next;
//             } else {
//                 previous->next = current->next;
//             }

//             gpiod_chip_close(current->chip);
//             free(current);

//             pthread_mutex_unlock(&led_list_mutex);
//             return LED_OK; 
//         }
//         previous = current;
//         current = current->next;
//     }

//     pthread_mutex_unlock(&led_list_mutex);
//     return LED_ERR;
// }


static int8_t led_request_output(const char *id, unsigned char default_val) {
    led_t *led = led_find_by_id(id);
    if (led) {
        if (gpiod_line_request_output(led->line, id, default_val) == 0) {
            led->val = default_val;
            return LED_OK;
        }
    }
    return LED_ERR;
}

static int8_t led_set_output(const char *id, unsigned int state) {
    led_t *led = led_find_by_id(id);
    if (led) {
        if ( gpiod_line_set_value(led->line, state) == 0) {
            led->val = state;
            return LED_OK;
        }
    }
    return LED_ERR;
}

// static void* blink_led(void* args) {
//     led_t *led = (led_t*) args;

//     while (led->should_blink) {
//         gpiod_line_set_value(led->line, 1);
//         usleep(led->delay);
//         gpiod_line_set_value(led->line, 0);
//         usleep(led->delay);
//     }
//     return NULL;
// }

// static int8_t led_set_tim_output(const char *id, unsigned int frequency) {

//     led_t *led = led_find_by_id(id);
//     if (led) {
//         led->should_blink = 1;
//         led->delay = frequency;
//         if (pthread_create(&led->blink_thread, NULL, blink_led, (void*) led) != 0) {
//             LED_LOG("pthread_create failed");
//             return LED_ERR;
//         }
//         return LED_OK;
//     }
//     return LED_ERR;
// }

// Methods from ::android::hardware::rpi::led::V1_0::ILed follow.
Return<int8_t> Led::init(const hidl_string& chipname, uint8_t line, const hidl_string& id) {
    // TODO implement
    led_t *led = led_init(chipname.c_str(), line, id.c_str()); 
    if(led == NULL) {
        return int8_t {-1};
    }
    return int8_t {0};
}

Return<int8_t> Led::requestOuput(const hidl_string& id) {
    // TODO implement
    return int8_t {led_request_output(id.c_str(), 0)};
}

Return<void> Led::setOuput(const hidl_string& id, uint8_t val) {
    // TODO implement
    led_set_output(id.c_str(), val);
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//ILed* HIDL_FETCH_ILed(const char* /* name */) {
    //return new Led();
//}
//
}  // namespace android::hardware::rpi::led::implementation
