#include <stdio.h>
#include <assert.h>
#include "pthipth_queue.h"
#include "pthipth_internal_type.h"

int main()
{
    int n = 10;
    pthipth_private_t t[n];

    pthipth_queue_t q;

    for (int i = 0; i < n; i++)
	t[i].tid = i + 1;

    for (int i = 0; i < n; i++)
	pthipth_queue_add(&q, &t[i]);

    pthipth_queue_display(&q);
    printf("\n");

    for (int i = 0; i < n; i++)
	pthipth_queue_delete(&q, &t[i]);

    pthipth_queue_display(&q);
    printf("\n");

    printf("success\n");

    return 0;
}
