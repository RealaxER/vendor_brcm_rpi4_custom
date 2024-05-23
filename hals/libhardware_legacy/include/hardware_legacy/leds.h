#ifndef LEDS_LEGACY_H
#define LEDS_LEGACY_H

#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <utils/Log.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEDS_ERR -1
#define LEDS_OK 1
#define LEDS_LOG(__va_args__) ALOGD(__va_args__)
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

led_t* leds_init(const char *chipname, unsigned int line_num, const char *led_id);
int leds_cleanup();
led_t* led_find_by_id(const char *led_id);
int led_remove_by_id(const char *led_id);
int leds_request_output(const char *id, unsigned char default_val);
int leds_set_output(const char *id, unsigned int state);
int leds_set_tim_output(const char *id, unsigned int frequency);
int leds_stop_blink(const char *id);

#ifdef __cplusplus
}
#endif

#endif /* LEDS_LEGACY_H */
