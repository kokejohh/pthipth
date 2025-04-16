// block and unblock signal for yield

#ifndef PTHIPTH_SIGNAL
#define PTHIPTH_SIGNAL

#include <signal.h>

extern sigset_t old_mask;

// should call once in first time.
#define __PTHIPTH_SIGNAL_INIT() \
    do { \
	sigset_t set; \
	sigemptyset (&set); \
	sigaddset(&set, SIGALRM); \
	sigprocmask(SIG_BLOCK, &set, &old_mask); \
    } while (0)

#define __PTHIPTH_SIGNAL_BLOCK() \
    do { \
	sigset_t set; \
	sigemptyset (&set); \
	sigaddset(&set, SIGALRM); \
	sigprocmask(SIG_BLOCK, &set, NULL); \
    } while (0)

#define __PTHIPTH_SIGNAL_UNBLOCK() \
    do { \
	sigprocmask(SIG_SETMASK, &old_mask, NULL); \
    } while (0)

#define __PTHIPTH_SIGNAL_PRINT() \
    do { \
	sigset_t current; \
	sigprocmask(SIG_SETMASK, NULL, &current); \
	printf("SIGALRM: %s\n", sigismember(&current, SIGALRM) ? "BLOCKED" : "UNBLOCKED"); \
    } while (0)

#endif
