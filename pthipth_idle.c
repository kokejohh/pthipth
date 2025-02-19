#include "pthipth.h"

extern pthipth_private_t *pthipth_q_head;

void *pthipth_idle(void *phony)
{
    pthipth_private_t *tmp;

    printf("idle thread start\n");
    while (1)
    {
	if (pthipth_q_head == NULL) 	
	    break;;
	pthipth_yield();
    }
    printf("idle thread end\n");
    return NULL;
}

