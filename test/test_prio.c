#include <stdio.h>
#include <assert.h>
#include "pthipth_prio.h"
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
	pthipth_prio_insert(&t[i]);

    pthipth_prio_display();
    printf("\n");

    pthipth_private_t *tmp = pthipth_prio_search(3);
    assert(tmp->tid == 3);

    pthipth_prio_rotate();
    tmp = pthipth_prio_peek();
    assert(tmp->tid == 6);

    for (int i = 0; i < n; i++)
	pthipth_prio_delete(&t[i]);

    pthipth_prio_display();
    printf("\n");

    tmp = pthipth_prio_search(3);

    assert(tmp == NULL);

    printf("success\n");

    return 0;
}
