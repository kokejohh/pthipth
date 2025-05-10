// block and unblock signal for yield

#ifndef PTHIPTH_SIGNAL
#define PTHIPTH_SIGNAL

#include <unistd.h>
#include <stddef.h>
#include <signal.h>

extern sigset_t pthipth_set;

// should call once in first time.
#define __PTHIPTH_SIGNAL_INIT() \
    do { \
	sigemptyset (&pthipth_set); \
	sigaddset(&pthipth_set, SIGALRM); \
    } while (0)

#define __PTHIPTH_SIGNAL_BLOCK() \
    do { sigprocmask(SIG_BLOCK, &pthipth_set, NULL); } while (0)

#define __PTHIPTH_SIGNAL_UNBLOCK() \
    do { sigprocmask(SIG_UNBLOCK, &pthipth_set, NULL); } while (0)

#define __PTHIPTH_SIGNAL_PRINT() \
    do { \
	sigset_t current; \
	sigprocmask(SIG_SETMASK, NULL, &current); \
	write(1, sigismember(&current, SIGALRM) ? "BLOCKED\n" : "UNBLOCK\n", 9); \
    } while (0)

#endif
