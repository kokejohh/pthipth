#include <unistd.h>
#include "pthipth.h"

void *__pthipth_idle(void *phony)
{
    (void)phony;

    while (1) pthipth_yield();

    return NULL;
}
