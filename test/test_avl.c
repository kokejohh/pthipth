#include <stdio.h>
#include <assert.h>
#include "pthipth_avl.h"
#include "pthipth_internal_type.h"

int main()
{
    int n = 10;
    pthipth_private_t t[n];

    for (int i = 0; i < n; i++)
	t[i].tid = i + 1;

    for (int i = 0; i < n; i++)
	pthipth_avl_insert(&t[i]);

    pthipth_avl_display();
    printf("\n");

    pthipth_private_t *tmp = pthipth_avl_search(3);
    assert(tmp->tid == 3);

    for (int i = 0; i < n; i++)
	pthipth_avl_delete(&t[i]);

    pthipth_avl_display();
    printf("\n");

    tmp = pthipth_avl_search(3);
    assert(tmp == NULL);

    printf("success\n");

    return 0;
}
