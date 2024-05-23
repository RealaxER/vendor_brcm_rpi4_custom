#define LOG_TAG "Leds"

#include <hardware_legacy/leds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <gpiod.h>
#include <unistd.h>

pthread_mutex_t led_list_mutex = PTHREAD_MUTEX_INITIALIZER;

led_t *led_list_head = NULL;

led_t* leds_init(const char *chipname, unsigned int line_num, const char *led_id) {

    led_t *new_led = (led_t *)malloc(sizeof(led_t));
    if (!new_led) {
        LEDS_LOG("Leds malloc failed");
        return NULL;
    }

    new_led->chip = gpiod_chip_open_by_name(chipname);
    if (!new_led->chip) {
        LEDS_LOG("Leds open chip failed");
        free(new_led);
        return NULL;
    }

    new_led->line = gpiod_chip_get_line(new_led->chip, line_num);
    if (!new_led->line) {
        LEDS_LOG("Leds get line failed");
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

int leds_cleanup() {
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

    return LEDS_OK;
}

led_t* led_find_by_id(const char *led_id) {
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

int led_remove_by_id(const char *led_id) {
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
            return LEDS_OK; 
        }
        previous = current;
        current = current->next;
    }

    pthread_mutex_unlock(&led_list_mutex);
    return LEDS_ERR;
}

int leds_request_output(const char *id, unsigned char default_val) {
    led_t *led = led_find_by_id(id);
    if (led) {
        if (gpiod_line_request_output(led->line, id, default_val) == 0) {
            led->val = default_val;
            return LEDS_OK;
        }
    }
    return LEDS_ERR;
}

int leds_set_output(const char *id, unsigned int state) {
    led_t *led = led_find_by_id(id);
    if (led) {
        if ( gpiod_line_set_value(led->line, state) == 0) {
            led->val = state;
            return LEDS_OK;
        }
    }
    return LEDS_ERR;
}

void* blink_led(void* args) {
    led_t *led = (led_t*) args;

    while (led->should_blink) {
        gpiod_line_set_value(led->line, 1);
        usleep(led->delay);
        gpiod_line_set_value(led->line, 0);
        usleep(led->delay);
    }
    return NULL;
}

int leds_set_tim_output(const char *id, unsigned int frequency) {
    pthread_mutex_lock(&led_list_mutex);

    led_t *led = led_find_by_id(id);
    if (led) {
        led->should_blink = 1;
        led->delay = frequency;
        if (pthread_create(&led->blink_thread, NULL, blink_led, (void*) led) != 0) {
            LEDS_LOG("pthread_create failed");
            return LEDS_ERR;
        }
        return LEDS_OK;
    }
    pthread_mutex_unlock(&led_list_mutex);
    return LEDS_ERR;
}

int leds_stop_blink(const char *id) {
    pthread_mutex_lock(&led_list_mutex);
    led_t *led = led_find_by_id(id);
    if (led) {
        led->should_blink = 0;
        pthread_join(led->blink_thread, NULL);
        return LEDS_OK;
    }
    pthread_mutex_unlock(&led_list_mutex);
    return LEDS_ERR;
}