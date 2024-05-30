#ifndef IR_REMOTE_LEGACY_H
#define IR_REMOTE_LEGACY_H


#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <utils/Log.h>


#ifdef __cplusplus
extern "C" {
#endif

#define IR_LOG(__va_args__) ALOGD(__va_args__)

int ir_init(const char *chipname, unsigned int line_num, const char *name);

#ifdef __cplusplus
}
#endif

#endif // IR REMOTE LEGARY