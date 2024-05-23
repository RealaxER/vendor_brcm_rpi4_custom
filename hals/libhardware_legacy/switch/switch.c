#define LOG_TAG "Switch"

//#include <hardware_legacy/switch.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <gpiod.h>
#include <unistd.h>
#include <pthread.h>

#define SW_ERR -1
#define SW_OK 1
#define SW_LOG(__va_args__) printf(__va_args__)

typedef struct sw {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    char id[32]; 
    struct sw *next;
    unsigned char val;
} sw_t;

pthread_mutex_t sw_list_mutex = PTHREAD_MUTEX_INITIALIZER;

sw_t *sw_list_head = NULL;

sw_t* sw_init(const char *chipname, unsigned int line_num, const char *sw_id) {

    sw_t *new_sw = (sw_t *)malloc(sizeof(sw_t));
    if (!new_sw) {
        SW_LOG("sw malloc faisw");
        return NULL;
    }

    new_sw->chip = gpiod_chip_open_by_name(chipname);
    if (!new_sw->chip) {
        SW_LOG("sw open chip faisw");
        free(new_sw);
        return NULL;
    }

    new_sw->line = gpiod_chip_get_line(new_sw->chip, line_num);
    if (!new_sw->line) {
        SW_LOG("sw get line faisw");
        gpiod_chip_close(new_sw->chip);
        free(new_sw);
        return NULL;
    }

    strncpy(new_sw->id, sw_id, sizeof(new_sw->id) - 1);
    new_sw->id[sizeof(new_sw->id) - 1] = '\0';

    pthread_mutex_lock(&sw_list_mutex);

    new_sw->next = sw_list_head;
    sw_list_head = new_sw;

    pthread_mutex_unlock(&sw_list_mutex);

    return new_sw;
}

int sw_cleanup() {
    pthread_mutex_lock(&sw_list_mutex);

    sw_t *current = sw_list_head;
    while (current != NULL) {
        sw_t *temp = current;
        current = current->next;

        gpiod_chip_close(temp->chip);
        free(temp);
    }

    sw_list_head = NULL;

    pthread_mutex_unlock(&sw_list_mutex);

    return SW_OK;
}


sw_t* sw_find_by_id(const char *sw_id) {
    pthread_mutex_lock(&sw_list_mutex);

    sw_t *current = sw_list_head;
    while (current != NULL) {
        if (strcmp(current->id, sw_id) == 0) {
            pthread_mutex_unlock(&sw_list_mutex);
            return current;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&sw_list_mutex);
    return NULL;
}

int sw_remove_by_id(const char *sw_id) {
    pthread_mutex_lock(&sw_list_mutex);

    sw_t *current = sw_list_head;
    sw_t *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->id, sw_id) == 0) {
            if (previous == NULL) {
                sw_list_head = current->next;
            } else {
                previous->next = current->next;
            }

            gpiod_chip_close(current->chip);
            free(current);

            pthread_mutex_unlock(&sw_list_mutex);
            return SW_OK; 
        }
        previous = current;
        current = current->next;
    }

    pthread_mutex_unlock(&sw_list_mutex);
    return SW_ERR;
}

int sw_request_input(const char *id) {
    sw_t *sw = sw_find_by_id(id);
    if (sw) {
        if (gpiod_line_request_input(sw->line, id) == 0) {
            return SW_OK;
        }
    }
    return SW_ERR;
}

int sw_get_input(const char *id) {
    sw_t *sw = sw_find_by_id(id);
    if (sw) {
        int value = gpiod_line_get_value(sw->line);
        return value;
    }
    return SW_ERR;
}
