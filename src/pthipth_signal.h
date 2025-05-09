// block and unblock signal for yield

#ifndef PTHIPTH_SIGNAL
#define PTHIPTH_SIGNAL

#include <stddef.h>
#include <signal.h>

extern sigset_t set;

// should call once in first time.
#define __PTHIPTH_SIGNAL_INIT() \
    do { \
	sigemptyset (&set); \
	sigaddset(&set, SIGALRM); \
    } while (0)

#define __PTHIPTH_SIGNAL_BLOCK() \
    do { sigprocmask(SIG_BLOCK, &set, NULL); } while (0)

#define __PTHIPTH_SIGNAL_UNBLOCK() \
    do { sigprocmask(SIG_UNBLOCK, &set, NULL); } while (0)

#define __PTHIPTH_SIGNAL_PRINT() \
    do { \
	sigset_t current; \
	sigprocmask(SIG_SETMASK, NULL, &current); \
	write(1, sigismember(&current, SIGALRM) ? "BLOCKED" : "UNBLOCK", 8); \
    } while (0)

#endif
