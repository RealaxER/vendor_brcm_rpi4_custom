#ifndef LEDS_CONVENTIONAL_H
#define LEDS_CONVENTIONAL_H

#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <utils/Log.h>
#include <gpiod.h>
#include <sys/types.h>          // size_t, ssize_t
#include <hardware/hardware.h>  // hw_module_t, hw_device_t


/* Use this for C++ */
__BEGIN_DECLS

#define LED_ERR -1
#define LED_OK 1
#define LED_LOG(__va_args__) ALOGD(__va_args__)
#define LED_HARDWARE_MODULE_ID "led_conventional"

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

struct led_module_t {
    struct hw_module_t common;
};

struct led_device_t {
    struct hw_device_t common;
    ssize_t (*write)(char *, size_t);
    ssize_t (*read)(char *, size_t);
    led_t* (*init)(const char *, unsigned int, const char *);
    led_t* (*find_by_id)(const char *);
    size_t (*cleanup)();
    size_t (*remove_by_id)(const char *);
    size_t (*request_output)(const char *, unsigned char);
    size_t (*set_output)(const char *, unsigned int);
    size_t (*set_tim_output)(const char *, unsigned int);
    size_t (*stop_blink)(const char *);
};


__END_DECLS

#endif /* LEDS_LEGACY_H */
