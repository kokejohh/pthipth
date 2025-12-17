#include <stdio.h>
#include <assert.h>
#include "pthipth_bq.h"
#include "pthipth_internal_type.h"

int main()
{
    int n = 10;
    pthipth_private_t t[n];

    for (int i = 0; i < n; i++)
    {
	t[i].tid = i + 1;
	t[i].priority = (i % 5) + 1;
    }

    for (int i = 0; i < n; i++)
	pthipth_bq_insert(&t[i]);

    pthipth_bq_display();
    printf("\n");

    pthipth_private_t *tmp = pthipth_bq_search(3);
    assert(tmp->tid == 3);

    pthipth_bq_rotate();
    tmp = pthipth_bq_peek();
    assert(tmp->tid == 6);

    for (int i = 0; i < n; i++)
	pthipth_bq_delete(&t[i]);

    pthipth_bq_display();
    printf("\n");

    tmp = pthipth_bq_search(3);

    assert(tmp == NULL);

    printf("success\n");

    return 0;
}
