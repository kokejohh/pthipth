#include <stdio.h>
#include <stdarg.h>

#include <sys/select.h>

#include "pthipth.h"
#include "pthipth_signal.h"

int pthipth_scanf(const char *format, ...)
{
    fd_set readfds;
    struct timeval tv;

    while (1)
    {
	__PTHIPTH_SIGNAL_BLOCK();

	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

	switch (ready)
	{
	    case -1: return -1;
	    case 0: pthipth_yield(); break;
	    default: 
	    {
		va_list args;

		va_start(args, format);

		int ret = vscanf(format, args);

		va_end(args);

		__PTHIPTH_SIGNAL_UNBLOCK();

		return ret;
	    }
	}
    }
    return 0;
}
