#ifndef PTHIPTH_DEBUG_H
#define PTHIPTH_DEBUG_H

#include <string.h>
#include "futex.h"

extern char msg[1000];
extern futex_t debug_futex;
extern void __pthipth_debug_futex_init();

#ifdef PTHIPTH_DEBUG

#define DEBUG_PRINTF(...) \
    __pthipth_debug_futex_init(); \
    futex_down(&debug_futex); \
    sprintf(msg, __VA_ARGS__); \
    write(1, msg, strlen(msg)); \
    futex_up(&debug_futex);
#else

#define DEBUG_PRINTF(...) while(0);
    
#endif

#define ERROR_PRINTF(...) \
    __pthipth_debug_futex_init(); \
    futex_down(&debug_futex); \
    sprintf(msg, __VA_ARGS__); \
    write(1, msg, strlen(msg)); \
    futex_up(&debug_futex);

#define LOG_PRINTF(...) \
    __pthipth_debug_futex_init(); \
    futex_down(&debug_futex); \
    sprintf(msg, __VA_ARGS__); \
    write(1, msg, strlen(msg)); \
    futex_up(&debug_futex);

#endif

