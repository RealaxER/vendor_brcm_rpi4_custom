#ifndef SW_LEGACY_H
#define SW_LEGACY_H

#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

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

sw_t* sw_init(const char *chipname, unsigned int line_num, const char *sw_id);
int sw_cleanup();
sw_t* sw_find_by_id(const char *sw_id);
int sw_remove_by_id(const char *sw_id);
int sw_request_output(const char *id);
int sw_set_input(const char *id, unsigned int state);

#ifdef __cplusplus
}
#endif

#endif /* sw_LEGACY_H */
